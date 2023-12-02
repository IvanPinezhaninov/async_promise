/******************************************************************************
**
** Copyright (C) 2023 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the async_promise - which can be found at
** https://github.com/IvanPinezhaninov/async_promise/.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
** OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
** THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
******************************************************************************/

#ifndef ASYNC_PROMISE_H
#define ASYNC_PROMISE_H

// stl
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <vector>


namespace async
{

/**
 * @brief All functions rejected error.
 */
struct aggregate_error final : public std::exception
{
  explicit aggregate_error(std::vector<std::exception_ptr> errors) noexcept
    : errors{std::move(errors)}
  {}

  const char* what() const noexcept final
  {
    return "All functions rejected";
  }

  std::vector<std::exception_ptr> errors;
};


/**
 * @brief Function execution result.
 */
enum class settle_type
{
  resolved, //!< Function completed successfully.
  rejected, //!< Function completed with an error.
};


/**
 * @brief Result of @ref async::promise::all_settled call.
 */
template<typename Result>
struct settled final
{
  using result_type = Result;

  /**
   * @brief Constructor of resolved object.
   * @param result - Function call result.
   */
  explicit settled(Result result)
    : type{settle_type::resolved}
    , result{std::move(result)}
  {}

  /**
   * @brief Constructor of rejected object.
   * @param error - Function call error.
   */
  explicit settled(std::exception_ptr error)
    : type{settle_type::rejected}
    , error{std::move(error)}
  {}

  settled(const settled& other)
    : type{other.type}
  {
    if (settle_type::resolved == type)
      ::new(&result) Result{other.result};
    else
      ::new(&error) std::exception_ptr{other.error};
  }

  settled(settled&& other) noexcept
    : type{other.type}
  {
    if (settle_type::resolved == type)
      ::new(&result) Result{std::move(other.result)};
    else
      ::new(&error) std::exception_ptr{std::move(other.error)};
  }

  settled& operator=(const settled& other)
  {
    if (this != &other)
      settled{other}.swap(*this);
    return *this;
  }

  settled& operator=(settled&& other) noexcept
  {
    other.swap(*this);
    return *this;
  }

  void swap(settled &other) noexcept
  {
    auto tmp = std::move(*this);
    ::new(this) settled{std::move(other)};
    ::new(&other) settled{std::move(tmp)};
  }

  ~settled()
  {
    if (settle_type::resolved == type)
      result.~result_type();
    else
      error.~exception_ptr();
  }

  /**
   * @brief @ref settle_type
   */
  settle_type type;

  union
  {
    /**
     * @brief Function call result if resolved
     */
    result_type result;

    /**
     * @brief Function call error if rejected
     */
    std::exception_ptr error;
  };
};


/**
 * @brief Result of @ref async::promise::all_settled call.
 */
template<>
struct settled<void> final
{
  using result_type = void;

  settled()
    : type{settle_type::resolved}
  {}

  explicit settled(std::exception_ptr error)
    : type{settle_type::rejected}
    , error{std::move(error)}
  {}

  /**
   * @brief @ref settle_type
   */
  settle_type type;

  /**
   * @brief Function call error if rejected
   */
  std::exception_ptr error;
};


namespace internal
{

template<std::size_t... indexes>
struct index_sequence
{
  static std::size_t size() {
    return sizeof...(indexes);
  }
};


template<std::size_t index, std::size_t... indexes>
struct make_index_sequence_helper;


template<size_t... indexes>
struct make_index_sequence_helper<0, indexes...>
{
  using type = index_sequence<indexes...>;
};


template<std::size_t index, std::size_t... indexes>
struct make_index_sequence_helper
{
  using type = typename make_index_sequence_helper<index - 1, index - 1, indexes...>::type;
};


template<std::size_t length>
struct make_index_sequence : public make_index_sequence_helper<length>::type{};


template<typename Func, typename Tuple, std::size_t... I>
auto apply(Func&& func, Tuple&& tuple, index_sequence<I...>) -> decltype(func(std::get<I>(std::forward<Tuple>(tuple))...))
{
  return func(std::get<I>(std::forward<Tuple>(tuple))...);
}


template<typename Func, typename Tuple,
         typename Seq = make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>>
auto apply(Func&& func, Tuple&& tuple) -> decltype(apply(std::forward<Func>(func), std::forward<Tuple>(tuple), Seq{}))
{
  return apply(std::forward<Func>(func), std::forward<Tuple>(tuple), Seq{});
}


template<typename Result>
class task
{
  public:
    task() = default;
    task(const task<Result>&) = default;
    task(task<Result>&&) = default;
    task& operator=(const task<Result>&) = default;
    task& operator=(task<Result>&&) = default;
    virtual ~task() = default;
    virtual Result run() = 0;

  protected:
    template<typename T>
    void reserve(T&, std::size_t)
    {}

    template<typename T>
    void reserve(std::vector<T>& v, std::size_t n)
    {
      v.reserve(n);
    }
};


template<typename T>
using task_ptr = std::shared_ptr<task<T>>;


template<typename Result>
class promise_helper
{
  protected:
    void resolve()
    {
      try
      {
        m_promise.set_value();
      }
      catch(...)
      {}
    }

    template<typename Value>
    void resolve(Value&& val)
    {
      try
      {
        m_promise.set_value(std::forward<Value>(val));
      }
      catch(...)
      {}
    }

    virtual void reject(std::exception_ptr err)
    {
      try
      {
        m_promise.set_exception(std::move(err));
      }
      catch(...)
      {}
    }

    std::promise<Result> m_promise;
};


template<typename Result>
class resolve_task final : public task<Result>
{
  public:
    template<typename T>
    explicit resolve_task(T&& val)
      : m_val{std::forward<T>(val)}
    {}

    Result run() final
    {
      return m_val;
    }

  private:
    Result m_val;
};


template<>
class resolve_task<void> final : public task<void>
{
  public:
    void run() final
    {}
};


template<typename Result, typename Error>
class reject_task final : public task<Result>
{
  public:
    template<typename T>
    explicit reject_task(T&& error)
      : m_error{std::forward<T>(error)}
    {}

    Result run() final
    {
      std::rethrow_exception(std::make_exception_ptr(std::move(m_error)));
    }

  private:
    Error m_error;
};


template<typename Result, typename Func, typename... Args>
class initial_task final : public task<Result>
{
  public:
    template<typename Func_, typename... Args_>
    explicit initial_task(Func_&& func, Args_&&... args)
      : m_func{std::forward<Func_>(func)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      return apply(m_func, m_args);
    }

  private:
    Func m_func;
    std::tuple<Args...> m_args;
};


template<typename Func, typename... Args>
class initial_task<void, Func, Args...> final : public task<void>
{
  public:
    template<typename Func_, typename... Args_>
    explicit initial_task(Func_&& func, Args_&&... args)
      : m_func{std::forward<Func_>(func)}
      , m_args{std::forward<Args_>(args)...}
    {}

    void run() final
    {
      apply(m_func, m_args);
    }

  private:
    Func m_func;
    std::tuple<Args...> m_args;
};


template<typename Result, typename ParentResult>
class next_task : public task<Result>
{
  public:
    explicit next_task(task_ptr<ParentResult> parent)
      : m_parent{std::move(parent)}
    {}

  protected:
    task_ptr<ParentResult> m_parent;
};


template<typename Result, typename ParentResult, typename Func>
class then_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename T>
    then_task(task_ptr<ParentResult> parent, T&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<T>(func)}
    {}

    Result run() final
    {
      return m_func(this->m_parent->run());
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult, typename Func>
class then_task_void final : public next_task<Result, ParentResult>
{
  public:
    template<typename T>
    then_task_void(task_ptr<ParentResult> parent, T&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<T>(func)}
    {}

    Result run() final
    {
      this->m_parent->run();
      return m_func();
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult, typename Func>
class fail_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename T>
    fail_task(task_ptr<ParentResult> parent, T&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<T>(func)}
    {}

    Result run() final
    {
      try
      {
        return this->m_parent->run();
      }
      catch(...)
      {
        return m_func(std::current_exception());
      }
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult, typename Func>
class fail_task_void final : public next_task<Result, ParentResult>
{
  public:
    template<typename T>
    fail_task_void(task_ptr<ParentResult> parent, T&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<T>(func)}
    {}

    Result run() final
    {
      try
      {
        return this->m_parent->run();
      }
      catch(...)
      {
        return m_func();
      }
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult, typename Func>
class finally_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename T>
    finally_task(task_ptr<ParentResult> parent, T&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<T>(func)}
    {}

    Result run() final
    {
      try
      {
        this->m_parent->run();
      }
      catch(...)
      {}

      return m_func();
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_task final : public next_task<Result, ParentResult>
{
  public:
    all_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      using value_type = typename Result::value_type;
      std::vector<std::future<value_type>> futures;
      futures.reserve(m_funcs.size());

      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(std::move(func), rv)));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class all_task<void, ParentResult, Container, Func, Allocator> final : public next_task<void, ParentResult>
{
  public:
    all_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<void, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    void run() final
    {
      std::vector<std::future<void>> futures;
      futures.reserve(m_funcs.size());

      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(std::move(func), rv)));

      for (auto& future : futures)
      {
        try
        {
          future.get();
        }
        catch(...)
        {
          std::rethrow_exception(std::current_exception());
        }
      }
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_task_void final : public next_task<Result, ParentResult>
{
  public:
    all_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      using value_type = typename Result::value_type;
      std::vector<std::future<value_type>> futures;
      futures.reserve(m_funcs.size());

      this->m_parent->run();
      for (auto&& func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class all_task_void<void, ParentResult, Container, Func, Allocator> final : public next_task<void, ParentResult>
{
  public:
    all_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<void, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    void run() final
    {
      std::vector<std::future<void>> futures;
      futures.reserve(m_funcs.size());

      this->m_parent->run();
      for (auto&& func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      for (auto& future : futures)
      {
        try
        {
          future.get();
        }
        catch(...)
        {
          std::rethrow_exception(std::current_exception());
        }
      }
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult, typename FuncResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_settled_task final : public next_task<Result, ParentResult>
{
  public:
    all_settled_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      std::vector<std::future<FuncResult>> futures;
      futures.reserve(m_funcs.size());

      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(std::move(func), rv)));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
      {
        try
        {
          result.emplace_back(future.get());
        }
        catch(...)
        {
          result.emplace_back(std::current_exception());
        }
      }

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_settled_task<Result, ParentResult, void, Container, Func, Allocator> final
    : public next_task<Result, ParentResult>
{
  public:
    all_settled_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      std::vector<std::future<void>> futures;
      futures.reserve(m_funcs.size());

      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(std::move(func), rv)));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
      {
        try
        {
          future.get();
          result.emplace_back();
        }
        catch(...)
        {
          result.emplace_back(std::current_exception());
        }
      }

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult, typename FuncResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_settled_task_void final : public next_task<Result, ParentResult>
{
  public:
    all_settled_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      std::vector<std::future<FuncResult>> futures;
      futures.reserve(m_funcs.size());

      this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
      {
        try
        {
          result.emplace_back(future.get());
        }
        catch(...)
        {
          result.emplace_back(std::current_exception());
        }
      }

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_settled_task_void<Result, ParentResult, void, Container, Func, Allocator> final
    : public next_task<Result, ParentResult>
{
  public:
    all_settled_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      std::vector<std::future<void>> futures;
      futures.reserve(m_funcs.size());

      this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
      {
        try
        {
          future.get();
          result.emplace_back();
        }
        catch(...)
        {
          result.emplace_back(std::current_exception());
        }
      }

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class next_functions_task : public next_task<Result, ParentResult>, public promise_helper<Result>
{
  public:
    next_functions_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

  protected:
    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class any_task_base : public next_functions_task<Result, ParentResult, Container, Func, Allocator>
{
  public:
    any_task_base(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_functions_task<Result, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

    Result run() final
    {
      auto future = this->m_promise.get_future();
      std::vector<std::future<void>> futures;
      futures.reserve(this->m_funcs.size());
      m_errors.reserve(this->m_funcs.size());
      async_run(futures);

      try
      {
        return future.get();
      }
      catch(...)
      {
        std::rethrow_exception(std::current_exception());
      }
    }

  protected:
    void reject(std::exception_ptr err) final
    {
      std::lock_guard<std::mutex> lock{m_mutex};

      m_errors.push_back(std::move(err));
      if (m_errors.size() < this->m_funcs.size())
        return;

      next_functions_task<Result, ParentResult, Container, Func, Allocator>
          ::reject(std::make_exception_ptr(aggregate_error{std::move(m_errors)}));
    }

  private:
    virtual void async_run(std::vector<std::future<void>>& futures) = 0;

    std::vector<std::exception_ptr> m_errors;
    std::mutex m_mutex;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class any_task final : public any_task_base<Result, ParentResult, Container, Func, Allocator>
{
  public:
    any_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<Result, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = any_task<Result, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func), arg)));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        this->resolve(func(std::move(arg)));
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class any_task<void, ParentResult, Container, Func, Allocator> final
    : public any_task_base<void, ParentResult, Container, Func, Allocator>
{
  public:
    any_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<void, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = any_task<void, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func), arg)));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        func(std::move(arg));
        this->resolve();
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class any_task_void final : public any_task_base<Result, ParentResult, Container, Func, Allocator>
{
  public:
    any_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<Result, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = any_task_void<Result, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));
    }

    void call(Func func)
    {
      try
      {
        this->resolve(func());
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class any_task_void<void, ParentResult, Container, Func, Allocator> final
    : public any_task_base<void, ParentResult, Container, Func, Allocator>
{
  public:
    any_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<void, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = any_task_void<void, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));
    }

    void call(Func func)
    {
      try
      {
        func();
        this->resolve();
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class race_task_base : public next_functions_task<Result, ParentResult, Container, Func, Allocator>
{
  public:
    race_task_base(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_functions_task<Result, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

    Result run() final
    {
      auto future = this->m_promise.get_future();
      std::vector<std::future<void>> futures;
      futures.reserve(this->m_funcs.size());
      async_run(futures);

      try
      {
        return future.get();
      }
      catch(...)
      {
        std::rethrow_exception(std::current_exception());
      }
    }

  private:
    virtual void async_run(std::vector<std::future<void>>& futures) = 0;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class race_task final : public race_task_base<Result, ParentResult, Container, Func, Allocator>
{
  public:
    race_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : race_task_base<Result, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = race_task<Result, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func), arg)));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        this->resolve(func(std::move(arg)));
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class race_task<void, ParentResult, Container, Func, Allocator> final
    : public race_task_base<void, ParentResult, Container, Func, Allocator>
{
  public:
    race_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : race_task_base<void, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = race_task<void, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func), arg)));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        func(std::move(arg));
        this->resolve();
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class race_task_void final : public race_task_base<Result, ParentResult, Container, Func, Allocator>
{
  public:
    race_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : race_task_base<Result, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = race_task_void<Result, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));
    }

    void call(Func func)
    {
      try
      {
        this->resolve(func());
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class race_task_void<void, ParentResult, Container, Func, Allocator> final
    : public race_task_base<void, ParentResult, Container, Func, Allocator>
{
  public:
    race_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : race_task_base<void, ParentResult, Container, Func, Allocator>{std::move(parent), std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = race_task_void<void, ParentResult, Container, Func, Allocator>;
      for (auto func : this->m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));
    }

    void call(Func func)
    {
      try
      {
        func();
        this->resolve();
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class all_task_static final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit all_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      using value_type = typename Result::value_type;
      std::vector<std::future<value_type>> futures;
      futures.reserve(m_funcs.size());

      using task = all_task_static<Result, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    typename Result::value_type call(Func func)
    {
      return apply(std::move(func), m_args);
    }

    Container<Func, Allocator> m_funcs;
    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
class all_task_static<void, Container, Func, Allocator, Args...> final : public task<void>
{
  public:
    template<typename... Args_>
    explicit all_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    void run() final
    {
      std::vector<std::future<void>> futures;
      futures.reserve(m_funcs.size());

      using task = all_task_static<void, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));

      for (auto& future : futures)
      {
        try
        {
          future.get();
        }
        catch(...)
        {
          std::rethrow_exception(std::current_exception());
        }
      }
    }

  private:
    void call(Func func)
    {
      apply(std::move(func), m_args);
    }

    Container<Func, Allocator> m_funcs;
    std::tuple<Args...> m_args;
};


template<typename Result, typename FuncResult, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class all_settled_task_static final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit all_settled_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      std::vector<std::future<FuncResult>> futures;
      futures.reserve(m_funcs.size());

      using task = all_settled_task_static<Result, FuncResult, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
      {
        try
        {
          result.emplace_back(future.get());
        }
        catch(...)
        {
          result.emplace_back(std::current_exception());
        }
      }

      return result;
    }

  private:
    FuncResult call(Func func)
    {
      return apply(std::move(func), m_args);
    }

    Container<Func, Allocator> m_funcs;
    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class all_settled_task_static<Result, void, Container, Func, Allocator, Args...> final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit all_settled_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      std::vector<std::future<void>> futures;
      futures.reserve(m_funcs.size());

      using task = all_settled_task_static<Result, void, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));

      Result result;
      this->reserve(result, m_funcs.size());

      for (auto& future : futures)
      {
        try
        {
          future.get();
          result.emplace_back();
        }
        catch(...)
        {
          result.emplace_back(std::current_exception());
        }
      }

      return result;
    }

  private:
    void call(Func func)
    {
      apply(std::move(func), m_args);
    }

    Container<Func, Allocator> m_funcs;
    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container, typename Func, typename Allocator>
class any_task_static_base : public task<Result>, public promise_helper<Result>
{
  public:
    explicit any_task_static_base(Container<Func, Allocator> funcs)
      : m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto future = this->m_promise.get_future();
      std::vector<std::future<void>> futures;
      futures.reserve(this->m_funcs.size());
      m_errors.reserve(this->m_funcs.size());

      using task = any_task_static_base<Result, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));

      try
      {
        return future.get();
      }
      catch(...)
      {
        std::rethrow_exception(std::current_exception());
      }
    }

  protected:
    void reject(std::exception_ptr err)
    {
      std::lock_guard<std::mutex> lock{m_mutex};

      m_errors.push_back(std::move(err));
      if (m_errors.size() < this->m_funcs.size())
        return;

      promise_helper<Result>::reject(std::make_exception_ptr(aggregate_error{std::move(m_errors)}));
    }

  private:
    virtual void call(Func func) = 0;

    Container<Func, Allocator> m_funcs;
    std::vector<std::exception_ptr> m_errors;
    std::mutex m_mutex;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class any_task_static final : public any_task_static_base<Result, Container, Func, Allocator>
{
  public:
    template<typename... Args_>
    explicit any_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : any_task_static_base<Result, Container, Func, Allocator>{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void call(Func func) final
    {
      try
      {
        this->resolve(apply(std::move(func), m_args));
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }

    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
class any_task_static<void, Container, Func, Allocator, Args...> final
    : public any_task_static_base<void, Container, Func, Allocator>
{
  public:
    template<typename... Args_>
    explicit any_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : any_task_static_base<void, Container, Func, Allocator>{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void call(Func func) final
    {
      try
      {
        apply(std::move(func), m_args);
        this->resolve();
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }

    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator>
class race_task_static_base : public task<Result>, public promise_helper<Result>
{
  public:
    template<typename... Args_>
    explicit race_task_static_base(Container<Func, Allocator> funcs)
      : m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto future = this->m_promise.get_future();
      std::vector<std::future<void>> futures;
      futures.reserve(this->m_funcs.size());

      using task = race_task_static_base<Result, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::bind(&task::call, this, std::move(func))));

      try
      {
        return future.get();
      }
      catch(...)
      {
        std::rethrow_exception(std::current_exception());
      }
    }

  private:
    virtual void call(Func func) = 0;

    Container<Func, Allocator> m_funcs;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class race_task_static final : public race_task_static_base<Result, Container, Func, Allocator>
{
  public:
    template<typename... Args_>
    explicit race_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : race_task_static_base<Result, Container, Func, Allocator>{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void call(Func func) final
    {
      try
      {
        this->resolve(apply(std::move(func), m_args));
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }

    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
class race_task_static<void, Container, Func, Allocator, Args...> final
    : public race_task_static_base<void, Container, Func, Allocator>
{
  public:
    template<typename... Args_>
    explicit race_task_static(Container<Func, Allocator> funcs, Args_&&... args)
      : race_task_static_base<void, Container, Func, Allocator>{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void call(Func func) final
    {
      try
      {
        apply(std::move(func), m_args);
        this->resolve();
      }
      catch(...)
      {
        this->reject(std::current_exception());
      }
    }

    std::tuple<Args...> m_args;
};

} // namespace internal


/**
 * @brief Promise class.
 */
template<typename T>
class promise
{
  public:
    promise(const promise&) = default;
    promise(promise&&) = default;
    promise& operator=(const promise&) = default;
    promise& operator=(promise&&) = default;
    virtual ~promise() = default;


    /**
     * @brief Constructor to create a promise object with an initial function.
     * @param func - Initial function.
     * @param args - Optional function arguments.
     */
    template<typename Func, typename... Args, typename task = internal::initial_task<T, Func, Args...>>
    explicit promise(Func&& func, Args&&... args)
      : m_task{std::make_shared<task>(std::forward<Func>(func), std::forward<Args>(args)...)}
    {};


    /**
     * @brief Internal constructor, no need to use.
     * @param task - Promise task.
     */
    template<typename Task,
             typename = typename std::enable_if<std::is_convertible<Task, internal::task_ptr<T>>::value>::type>
    explicit promise(Task&& task)
      : m_task{std::forward<Task>(task)}
    {};


    /**
     * @brief Add a function to be called if the previous function was resolved.
     * @param func - Function that receives the result of the previous function.
     * @return Promise object.
     */
    template<typename Func, typename Arg = T,
             typename Result = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type>
    promise<Result> then(Func&& func) const
    {
      using task = internal::then_task<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a function to be called if the previous function was resolved.
     * @param func - Function that not receives any result of the previous function.
     * @return Promise object.
     */
    template<typename Func, typename Result = typename std::result_of<Func()>::type>
    promise<Result> then(Func&& func) const
    {
      using task = internal::then_task_void<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a function to be called if the previous function was rejected.
     * @param func - Function that receives an exception object of a rejected function.
     *               Must return a value of the same type as the previous function.
     * @return Promise object.
     */
    template<typename Func, typename Arg = std::exception_ptr,
             typename Result = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<std::is_same<Result, T>::value>::type>
    promise<Result> fail(Func&& func) const
    {
      using task = internal::fail_task<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a function to be called if the previous function was rejected.
     * @param func - Function that not receives any result of a rejected function.
     *               Must return a value of the same type as the previous function.
     * @return Promise object.
     */
    template<typename Func, typename Result = typename std::result_of<Func()>::type,
             typename = typename std::enable_if<std::is_same<Result, T>::value>::type>
    promise<Result> fail(Func&& func) const
    {
      using task = internal::fail_task_void<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a function to be called if the previous function was either resolved or rejected.
     * @param func - Function that not receives any result of the previous function.
     * @return Promise object.
     */
    template<typename Func, typename Result = typename std::result_of<Func()>::type>
    promise<Result> finally(Func&& func) const
    {
      using task = internal::finally_task<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of results or first rejection reason.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename FuncResult = typename std::result_of<Func(Arg)>::type,
             typename Result = Container<FuncResult, std::allocator<FuncResult>>,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_task<Result, Arg, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of results or first rejection reason.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename Result = Container<FuncResult, std::allocator<FuncResult>>,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_task_void<Result, T, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of results or first rejection reason.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename FuncResult = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
    promise<void> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_task<void, Arg, Container, Func, Allocator>;
      return promise<void>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of results or first rejection reason.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
    promise<void> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_task_void<void, void, Container, Func, Allocator>;
      return promise<void>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with a result or an error.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename FuncResult = typename std::result_of<Func(Arg)>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all_settled(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_settled_task<Result, Arg, FuncResult, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with a result or an error.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type,
             typename = typename std::true_type::type>
    promise<Result> all_settled(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_settled_task_void<Result, T, void, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with a result or an error.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename FuncResult = typename std::result_of<Func(Arg)>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type,
             typename = typename std::true_type::type>
    promise<Result> all_settled(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_settled_task<Result, Arg, void, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with a result or an error.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all_settled(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_settled_task_void<Result, T, FuncResult, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return first resolved result.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename Result = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type>
    promise<Result> any(Container<Func, Allocator> funcs) const
    {
      using task = internal::any_task<Result, Arg, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return first resolved result.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Result = typename std::result_of<Func()>::type>
    promise<Result> any(Container<Func, Allocator> funcs) const
    {
      using task = internal::any_task_void<Result, T, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return first resolved or rejected result.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename Result = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type>
    promise<Result> race(Container<Func, Allocator> funcs) const
    {
      using task = internal::race_task<Result, Arg, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return first resolved or rejected result.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Result = typename std::result_of<Func()>::type>
    promise<Result> race(Container<Func, Allocator> funcs) const
    {
      using task = internal::race_task_void<Result, T, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Run execution of a chain of functions
     * @return Future with the result of execution
     */
    std::future<T> run() const
    {
      return std::async(std::launch::async, std::bind(&promise<T>::run_impl, this, m_task));
    }

  private:
    T run_impl(internal::task_ptr<T> task) const
    {
      return task->run();
    }

    internal::task_ptr<T> m_task;
};


/**
 * @brief Promise class with static methods.
 */
template<typename T>
class static_promise
{
  public:
    /**
     * @brief Add an iterable of functions to be called.
     *        Return an iterable of results or first rejection reason.
     * @param funcs - Functions.
     * @param args - Function arguments.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
    static promise<T> all(Container<Func, Allocator> funcs, Args&&... args)
    {
      using task = internal::all_task_static<T, Container, Func, Allocator, Args...>;
      return promise<T>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
    }


    /**
     * @brief Add an iterable of functions to be called.
     *        Return an iterable of @ref settled objects with a result or an error.
     * @param funcs - Functions.
     * @param args - Function arguments.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
    static promise<T> all_settled(Container<Func, Allocator> funcs, Args&&... args)
    {
      using FuncResult = typename T::value_type::result_type;
      using task = internal::all_settled_task_static<T, FuncResult, Container, Func, Allocator, Args...>;
      return promise<T>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
    }


    /**
     * @brief Add an iterable of functions to be called.
     *        Return first resolved result.
     * @param funcs - Functions.
     * @param args - Function arguments.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
    static promise<T> any(Container<Func, Allocator> funcs, Args&&... args)
    {
      using task = internal::any_task_static<T, Container, Func, Allocator, Args...>;
      return promise<T>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
    }


    /**
     * @brief Add an iterable of functions to be called.
     *        Return first resolved or rejected result.
     * @param funcs - Functions.
     * @param args - Function arguments.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
    static promise<T> race(Container<Func, Allocator> funcs, Args&&... args)
    {
      using task = internal::race_task_static<T, Container, Func, Allocator, Args...>;
      return promise<T>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
    }


    /**
     * @brief Create promise with resolved state.
     * @param value - Any value.
     * @return Promise object.
     */
    template<typename Result>
    static promise<T> resolve(Result&& value)
    {
      using task = internal::resolve_task<T>;
      return promise<T>{std::make_shared<task>(std::forward<Result>(value))};
    }


    /**
     * @brief Create promise with resolved state.
     * @return Promise object.
     */
    static promise<T> resolve()
    {
      using task = internal::resolve_task<T>;
      return promise<T>{std::make_shared<task>()};
    }


    /**
     * @brief Create promise with rejected state.
     * @param error - Any error
     * @return Promise object.
     */
    template<typename Error>
    static promise<T> reject(Error&& error)
    {
      using task = internal::reject_task<T, Error>;
      return promise<T>{std::make_shared<task>(std::forward<Error>(error))};
    }
};

} // namespace async

#endif // ASYNC_PROMISE_H
