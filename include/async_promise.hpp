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

#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <tuple>
#include <type_traits>
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

  ~settled()
  {
    if (settle_type::resolved == type)
      result.~result_type();
    else
      error.~exception_ptr();
  }

  void swap(settled &other) noexcept
  {
    auto tmp = std::move(*this);
    ::new(this) settled<Result>{std::move(other)};
    ::new(&other) settled<Result>{std::move(tmp)};
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

  void swap(settled& other) noexcept
  {
    std::swap(*this, other);
  }

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

#if __cplusplus < 201703L

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


template<typename Func, typename... Args>
struct is_invocable
    : std::is_constructible<std::function<void(Args...)>
    , std::reference_wrapper<typename std::remove_reference<Func>::type>>
{};

#else
using std::apply;
using std::is_invocable;
#endif


template<typename Result>
struct task
{
  task() = default;
  task(const task&) = default;
  task(task&&) = default;
  task& operator=(const task&) = default;
  task& operator=(task&&) = default;
  virtual ~task() = default;

  virtual Result run() = 0;
};


template<typename T>
using task_ptr = std::shared_ptr<task<T>>;


struct promise_helper
{
  static void resolve(std::promise<void>& promise)
  {
    try
    {
      promise.set_value();
    }
    catch(...)
    {}
  }

  template<typename T, typename Value>
  static void resolve(std::promise<T>& promise, Value&& val)
  {
    try
    {
      promise.set_value(std::forward<Value>(val));
    }
    catch(...)
    {}
  }

  template<typename T>
  static void reject(std::promise<T>& promise, std::exception_ptr err)
  {
    try
    {
      promise.set_exception(std::move(err));
    }
    catch(...)
    {}
  }
};


struct vector_helper
{
  template<typename T>
  static std::vector<T> create_vector(std::size_t reserve_size)
  {
    std::vector<T> v;
    v.reserve(reserve_size);
    return v;
  }

  template<typename T>
  static void reserve(T&, std::size_t)
  {}

  template<typename T>
  static void reserve(std::vector<T>& v, std::size_t n)
  {
    v.reserve(n);
  }
};


template<typename T>
struct class_method_call_helper
{
  template<typename Method, typename Class, typename Tuple>
  static T call(Method&& method, Class* obj, Tuple&& tpl)
  {
    using tuple_t = typename std::decay<Tuple>::type;
    using impl_t = impl<Method, Class, Tuple, 0 == std::tuple_size<tuple_t>::value, std::tuple_size<tuple_t>::value>;
    return impl_t::call(std::forward<Method>(method), obj, std::forward<Tuple>(tpl));
  }

  template<typename Method, typename Class, typename Tuple, bool Enough, int TotalArgs, int... N>
  struct impl
  {
    static T call(Method&& method, Class* obj, Tuple&& tpl)
    {
      using impl_t = impl<Method, Class, Tuple, TotalArgs == 1 + sizeof...(N), TotalArgs, N..., sizeof...(N)>;
      return impl_t::call(std::forward<Method>(method), obj, std::forward<Tuple>(tpl));
    }
  };

  template<typename Method, typename Class, typename Tuple, int TotalArgs, int... N>
  struct impl<Method, Class, Tuple, true, TotalArgs, N...>
  {
    static T call(Method&& method, Class* obj, Tuple&& tpl)
    {
      return (obj->*method)(std::get<N>(std::forward<Tuple>(tpl))...);
    }
  };
};


template<typename Result, typename Method, typename Class, typename... Args>
class initial_class_task final : public task<Result>
{
  public:
    template<typename Method_, typename... Args_>
    initial_class_task(Method_&& method, Class* obj, Args_&&... args)
      : m_method{std::forward<Method_>(method)}
      , m_obj{obj}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      return class_method_call_helper<Result>::call(m_method, m_obj, m_args);
    }

  private:
    Method m_method;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<typename Method, typename Class, typename... Args>
class initial_class_task<void, Method, Class, Args...> final : public task<void>
{
  public:
    template<typename Method_, typename... Args_>
    initial_class_task(Method_&& method, Class* obj, Args_&&... args)
      : m_method{std::forward<Method_>(method)}
      , m_obj{obj}
      , m_args{std::forward<Args_>(args)...}
    {}

    void run() final
    {
      class_method_call_helper<void>::call(m_method, m_obj, m_args);
    }

  private:
    Method m_method;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<typename Result, typename Func, typename... Args>
class initial_func_task final : public task<Result>
{
  public:
    template<typename Func_, typename... Args_>
    explicit initial_func_task(Func_&& func, Args_&&... args)
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
class initial_func_task<void, Func, Args...> final : public task<void>
{
  public:
    template<typename Func_, typename... Args_>
    explicit initial_func_task(Func_&& func, Args_&&... args)
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


template<typename Result, typename ParentResult, typename Method, typename Class>
class then_class_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename Method_>
    then_class_task(task_ptr<ParentResult> parent, Method_&& method, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_method{std::forward<Method_>(method)}
      , m_obj{obj}
    {}

    Result run() final
    {
      return (m_obj->*m_method)(this->m_parent->run());
    }

  private:
    Method m_method;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename Method, typename Class>
class then_class_task_void final : public next_task<Result, ParentResult>
{
public:
    template<typename Method_>
    then_class_task_void(task_ptr<ParentResult> parent, Method_&& method, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_method{std::forward<Method_>(method)}
      , m_obj{obj}
    {}

    Result run() final
    {
      this->m_parent->run();
      return (m_obj->*m_method)();
    }

private:
    Method m_method;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename Func>
class then_func_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename Func_>
    then_func_task(task_ptr<ParentResult> parent, Func_&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<Func_>(func)}
    {}

    Result run() final
    {
      return m_func(this->m_parent->run());
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult, typename Func>
class then_func_task_void final : public next_task<Result, ParentResult>
{
  public:
    template<typename Func_>
    then_func_task_void(task_ptr<ParentResult> parent, Func_&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<Func_>(func)}
    {}

    Result run() final
    {
      this->m_parent->run();
      return m_func();
    }

  private:
    Func m_func;
};


template<typename Result, typename ParentResult, typename Method, typename Class>
class fail_class_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename Method_>
    fail_class_task(task_ptr<ParentResult> parent, Method_&& method, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_method{std::forward<Method_>(method)}
      , m_obj{obj}
    {}

    Result run() final
    {
      try
      {
        return this->m_parent->run();
      }
      catch(...)
      {
        return (m_obj->*m_method)(std::current_exception());
      }
    }

  private:
    Method m_method;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename Method, typename Class>
class fail_class_task_void final : public next_task<Result, ParentResult>
{
  public:
    template<typename Method_>
    fail_class_task_void(task_ptr<ParentResult> parent, Method_&& method, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_method{std::forward<Method_>(method)}
      , m_obj{obj}
    {}

    Result run() final
    {
      try
      {
        return this->m_parent->run();
      }
      catch(...)
      {
        return (m_obj->*m_method)();
      }
    }

  private:
    Method m_method;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename Func>
class fail_func_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename Method_>
    fail_func_task(task_ptr<ParentResult> parent, Method_&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<Method_>(func)}
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
class fail_func_task_void final : public next_task<Result, ParentResult>
{
  public:
    template<typename Func_>
    fail_func_task_void(task_ptr<ParentResult> parent, Func_&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<Func_>(func)}
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


template<typename Result, typename ParentResult, typename Method, typename Class>
class finally_class_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename Method_>
    finally_class_task(task_ptr<ParentResult> parent, Method_&& method, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_method{std::forward<Method_>(method)}
      , m_obj{obj}
    {}

    Result run() final
    {
      try
      {
        this->m_parent->run();
      }
      catch(...)
      {}

      return (m_obj->*m_method)();
    }

  private:
    Method m_method;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename Func>
class finally_func_task final : public next_task<Result, ParentResult>
{
  public:
    template<typename Func_>
    finally_func_task(task_ptr<ParentResult> parent, Func_&& func)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_func{std::forward<Func_>(func)}
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


template<typename Result, typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class all_class_task final : public next_task<Result, ParentResult>
{
  public:
    all_class_task(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<typename Result::value_type>>(m_methods.size());
      auto rv = this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj, rv));

      Result result;
      vector_helper::reserve(result, m_methods.size());
      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class all_class_task<void, ParentResult, Container, Method, Allocator, Class> final
    : public next_task<void, ParentResult>
{
  public:
    all_class_task(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<void, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    void run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_methods.size());
      auto rv = this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj, rv));

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
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Method, typename Allocator, typename Class>
class all_class_task_void final : public next_task<Result, ParentResult>
{
  public:
    all_class_task_void(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<typename Result::value_type>>(m_methods.size());
      this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj));

      Result result;
      vector_helper::reserve(result, m_methods.size());
      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class all_class_task_void<void, ParentResult, Container, Method, Allocator, Class> final
    : public next_task<void, ParentResult>
{
  public:
    all_class_task_void(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<void, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    void run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_methods.size());
      this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj));

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
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_func_task final : public next_task<Result, ParentResult>
{
  public:
    all_func_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<typename Result::value_type>>(m_funcs.size());
      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func), rv));

      Result result;
      vector_helper::reserve(result, m_funcs.size());
      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class all_func_task<void, ParentResult, Container, Func, Allocator> final : public next_task<void, ParentResult>
{
  public:
    all_func_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<void, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    void run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func), rv));

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
class all_func_task_void final : public next_task<Result, ParentResult>
{
  public:
    all_func_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<typename Result::value_type>>(m_funcs.size());
      this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      Result result;
      vector_helper::reserve(result, m_funcs.size());
      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    Container<Func, Allocator> m_funcs;
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class all_func_task_void<void, ParentResult, Container, Func, Allocator> final : public next_task<void, ParentResult>
{
  public:
    all_func_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<void, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    void run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      this->m_parent->run();
      for (auto func : m_funcs)
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


template<typename Result, typename ParentResult, typename MethodResult,
template<typename, typename> class Container, typename Method, typename Allocator, typename Class>
class all_settled_class_task final : public next_task<Result, ParentResult>
{
  public:
    all_settled_class_task(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<MethodResult>>(m_methods.size());
      auto rv = this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj, rv));

      Result result;
      vector_helper::reserve(result, m_methods.size());

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
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Method, typename Allocator, typename Class>
class all_settled_class_task<Result, ParentResult, void, Container, Method, Allocator, Class> final
    : public next_task<Result, ParentResult>
{
  public:
    all_settled_class_task(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_methods.size());
      auto rv = this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj, rv));

      Result result;
      vector_helper::reserve(result, m_methods.size());

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
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename MethodResult,
         template<typename, typename> class Container, typename Method, typename Allocator, typename Class>
class all_settled_class_task_void final : public next_task<Result, ParentResult>
{
  public:
    all_settled_class_task_void(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<MethodResult>>(m_methods.size());
      this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj));

      Result result;
      vector_helper::reserve(result, m_methods.size());

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
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Method, typename Allocator, typename Class>
class all_settled_class_task_void<Result, ParentResult, void, Container, Method, Allocator, Class> final
    : public next_task<Result, ParentResult>
{
  public:
    all_settled_class_task_void(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_methods.size());
      this->m_parent->run();
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, std::move(method), m_obj));

      Result result;
      vector_helper::reserve(result, m_methods.size());

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
    Container<Method, Allocator> m_methods;
    Class* const m_obj;
};


template<typename Result, typename ParentResult, typename FuncResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class all_settled_func_task final : public next_task<Result, ParentResult>
{
  public:
    all_settled_func_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<FuncResult>>(m_funcs.size());
      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func), rv));

      Result result;
      vector_helper::reserve(result, m_funcs.size());

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
class all_settled_func_task<Result, ParentResult, void, Container, Func, Allocator> final
    : public next_task<Result, ParentResult>
{
  public:
    all_settled_func_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      auto rv = this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func), rv));

      Result result;
      vector_helper::reserve(result, m_funcs.size());

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
class all_settled_func_task_void final : public next_task<Result, ParentResult>
{
  public:
    all_settled_func_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<FuncResult>>(m_funcs.size());
      this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      Result result;
      vector_helper::reserve(result, m_funcs.size());

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
class all_settled_func_task_void<Result, ParentResult, void, Container, Func, Allocator> final
    : public next_task<Result, ParentResult>
{
  public:
    all_settled_func_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      this->m_parent->run();
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, std::move(func)));

      Result result;
      vector_helper::reserve(result, m_funcs.size());

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


template<typename Result, typename ParentResult>
class any_task_base : public next_task<Result, ParentResult>
{
  public:
    any_task_base(task_ptr<ParentResult> parent)
      : next_task<Result, ParentResult>{std::move(parent)}
    {}

    Result run() final
    {
      auto future = m_promise.get_future();
      auto futures = vector_helper::create_vector<std::future<void>>(iterable_size());
      m_errors.reserve(iterable_size());
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
    void process_error(std::exception_ptr err)
    {
      std::lock_guard<std::mutex> lock{m_mutex};

      m_errors.push_back(std::move(err));
      if (m_errors.size() < iterable_size())
        return;

      promise_helper::reject(m_promise, std::make_exception_ptr(aggregate_error{std::move(m_errors)}));
    }

    std::promise<Result> m_promise;

  private:
    virtual void async_run(std::vector<std::future<void>>& futures) = 0;
    virtual std::size_t iterable_size() const = 0;

    std::vector<std::exception_ptr> m_errors;
    std::mutex m_mutex;
};


template<typename Result, typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class any_class_task final : public any_task_base<Result, ParentResult>
{
  public:
    any_class_task(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : any_task_base<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = any_class_task<Result, ParentResult, Container, Method, Allocator, Class>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method), arg));
    }

    void call(Method method, ParentResult arg)
    {
      try
      {
        promise_helper::resolve(this->m_promise, (m_obj->*method)(std::move(arg)));
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_methods.size();
    }

    Container<Method, Allocator> m_methods;
    Class* m_obj;
};


template<typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class any_class_task<void, ParentResult, Container, Method, Allocator, Class> final
    : public any_task_base<void, ParentResult>
{
  public:
    any_class_task(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : any_task_base<void, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = any_class_task<void, ParentResult, Container, Method, Allocator, Class>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method), arg));
    }

    void call(Method method, ParentResult arg)
    {
      try
      {
        (m_obj->*method)(std::move(arg));
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_methods.size();
    }

    Container<Method, Allocator> m_methods;
    Class* m_obj;
};


template<typename Result, typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class any_class_task_void final : public any_task_base<Result, ParentResult>
{
  public:
    any_class_task_void(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : any_task_base<Result, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = any_class_task_void<Result, ParentResult, Container, Method, Allocator, Class>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));
    }

    void call(Method method)
    {
      try
      {
        promise_helper::resolve(this->m_promise, (m_obj->*method)());
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_methods.size();
    }

    Container<Method, Allocator> m_methods;
    Class* m_obj;
};


template<typename ParentResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class>
class any_class_task_void<void, ParentResult, Container, Method, Allocator, Class> final
    : public any_task_base<void, ParentResult>
{
  public:
    any_class_task_void(task_ptr<ParentResult> parent, Container<Method, Allocator> methods, Class* obj)
      : any_task_base<void, ParentResult>{std::move(parent)}
      , m_methods{std::move(methods)}
      , m_obj{obj}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = any_class_task_void<void, ParentResult, Container, Method, Allocator, Class>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));
    }

    void call(Method method)
    {
      try
      {
        (m_obj->*method)();
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_methods.size();
    }

    Container<Method, Allocator> m_methods;
    Class* m_obj;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class any_func_task final : public any_task_base<Result, ParentResult>
{
  public:
    any_func_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = any_func_task<Result, ParentResult, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func), arg));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        promise_helper::resolve(this->m_promise, func(std::move(arg)));
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_funcs.size();
    }

    Container<Func, Allocator> m_funcs;
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class any_func_task<void, ParentResult, Container, Func, Allocator> final
    : public any_task_base<void, ParentResult>
{
  public:
    any_func_task(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<void, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      auto arg = this->m_parent->run();
      using task = any_func_task<void, ParentResult, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func), arg));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        func(std::move(arg));
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_funcs.size();
    }

    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class any_func_task_void final : public any_task_base<Result, ParentResult>
{
  public:
    any_func_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = any_func_task_void<Result, ParentResult, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));
    }

    void call(Func func)
    {
      try
      {
        promise_helper::resolve(this->m_promise, func());
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_funcs.size();
    }

    Container<Func, Allocator> m_funcs;
};


template<typename ParentResult, template<typename, typename> class Container, typename Func, typename Allocator>
class any_func_task_void<void, ParentResult, Container, Func, Allocator> final
    : public any_task_base<void, ParentResult>
{
  public:
    any_func_task_void(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : any_task_base<void, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      this->m_parent->run();
      using task = any_func_task_void<void, ParentResult, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));
    }

    void call(Func func)
    {
      try
      {
        func();
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_funcs.size();
    }

    Container<Func, Allocator> m_funcs;
};


template<typename Result, typename ParentResult,
         template<typename, typename> class Container, typename Func, typename Allocator>
class race_task_base : public next_task<Result, ParentResult>
{
  public:
    race_task_base(task_ptr<ParentResult> parent, Container<Func, Allocator> funcs)
      : next_task<Result, ParentResult>{std::move(parent)}
      , m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto future = m_promise.get_future();
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
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
    Container<Func, Allocator> m_funcs;
    std::promise<Result> m_promise;

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
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func), arg));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        promise_helper::resolve(this->m_promise, func(std::move(arg)));
      }
      catch(...)
      {
        promise_helper::reject(this->m_promise, std::current_exception());
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
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func), arg));
    }

    void call(Func func, ParentResult arg)
    {
      try
      {
        func(std::move(arg));
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        promise_helper::reject(this->m_promise, std::current_exception());
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
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));
    }

    void call(Func func)
    {
      try
      {
        promise_helper::resolve(this->m_promise, func());
      }
      catch(...)
      {
        promise_helper::reject(this->m_promise, std::current_exception());
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
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));
    }

    void call(Func func)
    {
      try
      {
        func();
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        promise_helper::reject(this->m_promise, std::current_exception());
      }
    }
};


template<typename Result, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class, typename... Args>
class make_all_class_task final : public task<Result>
{
  public:
    template<typename... Args_>
    make_all_class_task(Container<Method, Allocator> methods, Class* obj, Args_&&... args)
      : m_methods{std::move(methods)}
      , m_obj{obj}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<typename Result::value_type>>(m_methods.size());
      using task = make_all_class_task<Result, Container, Method, Allocator, Class, Args...>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));

      Result result;
      vector_helper::reserve(result, m_methods.size());
      for (auto& future : futures)
        result.push_back(future.get());

      return result;
    }

  private:
    typename Result::value_type call(Method method) const
    {
      return class_method_call_helper<typename Result::value_type>::call(std::move(method), m_obj, m_args);
    }

    Container<Method, Allocator> m_methods;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Method,
         typename Allocator, typename Class, typename... Args>
class make_all_class_task<void, Container, Method, Allocator, Class, Args...> final : public task<void>
{
  public:
    template<typename... Args_>
    make_all_class_task(Container<Method, Allocator> methods, Class* obj, Args_&&... args)
      : m_methods{std::move(methods)}
      , m_obj{obj}
      , m_args{std::forward<Args_>(args)...}
    {}

    void run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_methods.size());
      using task = make_all_class_task<void, Container, Method, Allocator, Class, Args...>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));

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
    void call(Method method) const
    {
      class_method_call_helper<void>::call(std::move(method), m_obj, m_args);
    }

    Container<Method, Allocator> m_methods;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class make_all_func_task final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit make_all_func_task(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<typename Result::value_type>>(m_funcs.size());
      using task = make_all_func_task<Result, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));

      Result result;
      vector_helper::reserve(result, m_funcs.size());
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
class make_all_func_task<void, Container, Func, Allocator, Args...> final : public task<void>
{
  public:
    template<typename... Args_>
    explicit make_all_func_task(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    void run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      using task = make_all_func_task<void, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));

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


template<typename Result, typename MethodResult, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class, typename... Args>
class make_all_settled_class_task final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit make_all_settled_class_task(Container<Method, Allocator> methods, Class* obj, Args_&&... args)
      : m_methods{std::move(methods)}
      , m_args{std::forward<Args_>(args)...}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<MethodResult>>(m_methods.size());
      using task = make_all_settled_class_task<Result, MethodResult, Container, Method, Allocator, Class, Args...>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));

      Result result;
      vector_helper::reserve(result, m_methods.size());

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
    MethodResult call(Method method)
    {
      return class_method_call_helper<MethodResult>::call(std::move(method), m_obj, m_args);
    }

    Container<Method, Allocator> m_methods;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class, typename... Args>
class make_all_settled_class_task<Result, void, Container, Method, Allocator, Class, Args...> final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit make_all_settled_class_task(Container<Method, Allocator> methods, Class* obj, Args_&&... args)
      : m_methods{std::move(methods)}
      , m_args{std::forward<Args_>(args)...}
      , m_obj{obj}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_methods.size());
      using task = make_all_settled_class_task<Result, void, Container, Method, Allocator, Class, Args...>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));

      Result result;
      vector_helper::reserve(result, m_methods.size());

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
    void call(Method method)
    {
      class_method_call_helper<void>::call(std::move(method), m_obj, m_args);
    }

    Container<Method, Allocator> m_methods;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<typename Result, typename FuncResult, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class make_all_settled_func_task final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit make_all_settled_func_task(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<FuncResult>>(m_funcs.size());
      using task = make_all_settled_func_task<Result, FuncResult, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));

      Result result;
      vector_helper::reserve(result, m_funcs.size());

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
class make_all_settled_func_task<Result, void, Container, Func, Allocator, Args...> final : public task<Result>
{
  public:
    template<typename... Args_>
    explicit make_all_settled_func_task(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

    Result run() final
    {
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      using task = make_all_settled_func_task<Result, void, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));

      Result result;
      vector_helper::reserve(result, m_funcs.size());

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


template<typename Result>
class make_any_task_base : public task<Result>
{
  public:
    Result run() final
    {
      auto future = m_promise.get_future();
      auto futures = vector_helper::create_vector<std::future<void>>(iterable_size());
      m_errors.reserve(iterable_size());
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
    void process_error(std::exception_ptr err)
    {
      std::lock_guard<std::mutex> lock{m_mutex};

      m_errors.push_back(std::move(err));
      if (m_errors.size() < iterable_size())
        return;

      promise_helper::reject(m_promise, std::make_exception_ptr(aggregate_error{std::move(m_errors)}));
    }

    std::promise<Result> m_promise;

  private:
    virtual void async_run(std::vector<std::future<void>>& futures) = 0;
    virtual std::size_t iterable_size() const = 0;

    std::vector<std::exception_ptr> m_errors;
    std::mutex m_mutex;
};


template<typename Result, template<typename, typename> class Container,
         typename Method, typename Allocator, typename Class, typename... Args>
class make_any_class_task final : public make_any_task_base<Result>
{
  public:
    template<typename... Args_>
    explicit make_any_class_task(Container<Method, Allocator> methods, Class* obj, Args_&&... args)
      : m_methods{std::move(methods)}
      , m_obj{obj}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      using task = make_any_class_task<Result, Container, Method, Allocator, Class, Args...>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));
    }

    void call(Method method)
    {
      try
      {
        auto val = class_method_call_helper<Result>::call(std::move(method), m_obj, m_args);
        promise_helper::resolve(this->m_promise, std::move(val));
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_methods.size();
    }

    Container<Method, Allocator> m_methods;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Method,
         typename Allocator, typename Class, typename... Args>
class make_any_class_task<void, Container, Method, Allocator, Class, Args...> final : public make_any_task_base<void>
{
  public:
    template<typename... Args_>
    explicit make_any_class_task(Container<Method, Allocator> methods, Class* obj, Args_&&... args)
      : m_methods{std::move(methods)}
      , m_obj{obj}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      using task = make_any_class_task<void, Container, Method, Allocator, Class, Args...>;
      for (auto method : m_methods)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(method)));
    }

    void call(Method method)
    {
      try
      {
        class_method_call_helper<void>::call(std::move(method), m_obj, m_args);
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_methods.size();
    }

    Container<Method, Allocator> m_methods;
    Class* const m_obj;
    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class make_any_func_task final : public make_any_task_base<Result>
{
  public:
    template<typename... Args_>
    explicit make_any_func_task(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      using task = make_any_func_task<Result, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));
    }

    void call(Func func)
    {
      try
      {
        promise_helper::resolve(this->m_promise, apply(std::move(func), m_args));
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_funcs.size();
    }

    Container<Func, Allocator> m_funcs;
    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
class make_any_func_task<void, Container, Func, Allocator, Args...> final : public make_any_task_base<void>
{
  public:
    template<typename... Args_>
    explicit make_any_func_task(Container<Func, Allocator> funcs, Args_&&... args)
      : m_funcs{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void async_run(std::vector<std::future<void>>& futures) final
    {
      using task = make_any_func_task<void, Container, Func, Allocator, Args...>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));
    }

    void call(Func func)
    {
      try
      {
        apply(std::move(func), m_args);
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        this->process_error(std::current_exception());
      }
    }

    std::size_t iterable_size() const final
    {
      return m_funcs.size();
    }

    Container<Func, Allocator> m_funcs;
    std::tuple<Args...> m_args;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator>
class race_task_static_base : public task<Result>
{
  public:
    template<typename... Args_>
    explicit race_task_static_base(Container<Func, Allocator> funcs)
      : m_funcs{std::move(funcs)}
    {}

    Result run() final
    {
      auto future = m_promise.get_future();
      auto futures = vector_helper::create_vector<std::future<void>>(m_funcs.size());
      using task = race_task_static_base<Result, Container, Func, Allocator>;
      for (auto func : m_funcs)
        futures.push_back(std::async(std::launch::async, &task::call, this, std::move(func)));

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
    std::promise<Result> m_promise;

  private:
    virtual void call(Func func) = 0;

    Container<Func, Allocator> m_funcs;
};


template<typename Result, template<typename, typename> class Container,
         typename Func, typename Allocator, typename... Args>
class make_race_task final : public race_task_static_base<Result, Container, Func, Allocator>
{
  public:
    template<typename... Args_>
    explicit make_race_task(Container<Func, Allocator> funcs, Args_&&... args)
      : race_task_static_base<Result, Container, Func, Allocator>{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void call(Func func) final
    {
      try
      {
        promise_helper::resolve(this->m_promise, apply(std::move(func), m_args));
      }
      catch(...)
      {
        promise_helper::reject(this->m_promise, std::current_exception());
      }
    }

    std::tuple<Args...> m_args;
};


template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args>
class make_race_task<void, Container, Func, Allocator, Args...> final
    : public race_task_static_base<void, Container, Func, Allocator>
{
  public:
    template<typename... Args_>
    explicit make_race_task(Container<Func, Allocator> funcs, Args_&&... args)
      : race_task_static_base<void, Container, Func, Allocator>{std::move(funcs)}
      , m_args{std::forward<Args_>(args)...}
    {}

  private:
    void call(Func func) final
    {
      try
      {
        apply(std::move(func), m_args);
        promise_helper::resolve(this->m_promise);
      }
      catch(...)
      {
        promise_helper::reject(this->m_promise, std::current_exception());
      }
    }

    std::tuple<Args...> m_args;
};


template<typename Result>
class make_resolved_task final : public task<Result>
{
  public:
    template<typename T>
    explicit make_resolved_task(T&& val)
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
class make_resolved_task<void> final : public task<void>
{
  public:
    void run() final
    {}
};


template<typename Result, typename Error>
class make_rejected_task final : public task<Result>
{
  public:
    template<typename T>
    explicit make_rejected_task(T&& error)
      : m_error{std::forward<T>(error)}
    {}

    Result run() final
    {
      std::rethrow_exception(std::make_exception_ptr(std::move(m_error)));
    }

  private:
    Error m_error;
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
     * @brief Constructor to create a promise object with an initial class method.
     * @param method - Method for call.
     * @param obj - Object containing the required method.
     * @param args - Optional arguments.
     */
    template<typename Method, typename Class, typename... Args,
             typename task = internal::initial_class_task<T, Method, Class, Args...>,
             typename = typename std::enable_if<internal::is_invocable<Method, Class, Args...>::value>::type>
    promise(Method&& method, Class* obj, Args&&... args)
      : m_task{std::make_shared<task>(std::forward<Method>(method), obj, std::forward<Args>(args)...)}
    {};


    /**
     * @brief Constructor to create a promise object with an initial function.
     * @param func - Initial function.
     * @param args - Optional arguments.
     */
    template<typename Func, typename... Args,
             typename task = internal::initial_func_task<T, Func, Args...>>
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
     * @brief Add a class method to be called if the previous function was resolved.
     * @param method - Method that not receives any result of the previous call.
     * @param obj - Object containing the required method.
     * @return Promise object.
     */
    template<typename Method, typename Class, typename Result = typename std::result_of<Method(Class*)>::type>
    promise<Result> then(Method&& method, Class* obj) const
    {
      using task = internal::then_class_task_void<Result, T, Method, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Method>(method), obj)};
    }


    /**
     * @brief Add a class method to be called if the previous function was resolved.
     * @param method - Method that receives the result of the previous call.
     * @param obj - Object containing the required method.
     * @return Promise object.
     */
    template<typename Method, typename Class, typename Arg = T,
             typename Result = typename std::result_of<Method(Class*, Arg)>::type>
    promise<Result> then(Method&& method, Class* obj) const
    {
      using task = internal::then_class_task<Result, T, Method, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Method>(method), obj)};
    }


    /**
     * @brief Add a function to be called if the previous function was resolved.
     * @param func - Function that not receives any result of the previous call.
     * @return Promise object.
     */
    template<typename Func, typename Result = typename std::result_of<Func()>::type>
    promise<Result> then(Func&& func) const
    {
      using task = internal::then_func_task_void<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a function to be called if the previous function was resolved.
     * @param func - Function that receives the result of the previous call.
     * @return Promise object.
     */
    template<typename Func, typename Arg = T,
             typename Result = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type>
    promise<Result> then(Func&& func) const
    {
      using task = internal::then_func_task<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a class method to be called if the previous function was rejected.
     * @param method - Method that receives an exception object of a rejected function.
     *                 Must return a value of the same type as the previous function.
     * @param obj - Object containing the required method.
     * @return Promise object.
     */
    template<typename Method, typename Class, typename Arg = std::exception_ptr,
             typename Result = typename std::result_of<Method(Class*, Arg)>::type,
             typename = typename std::enable_if<std::is_same<Result, T>::value>::type>
    promise<Result> fail(Method&& method, Class* obj) const
    {
      using task = internal::fail_class_task<Result, T, Method, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Method>(method), obj)};
    }


    /**
     * @brief Add a class method to be called if the previous function was rejected.
     * @param method - Method that not receives any result of a rejected function.
     *                 Must return a value of the same type as the previous function.
     * @param obj - Object containing the required method.
     * @return Promise object.
     */
    template<typename Method, typename Class,
             typename Result = typename std::result_of<Method(Class*)>::type,
             typename = typename std::enable_if<std::is_same<Result, T>::value>::type>
    promise<Result> fail(Method&& method, Class* obj) const
    {
      using task = internal::fail_class_task_void<Result, T, Method, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Method>(method), obj)};
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
      using task = internal::fail_func_task<Result, T, Func>;
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
      using task = internal::fail_func_task_void<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add a class method to be called if the previous function was either resolved or rejected.
     * @param method - Method that not receives any result of the previous function.
     * @param obj - Object containing the required method.
     * @return Promise object.
     */
    template<typename Method, typename Class, typename Result = typename std::result_of<Method(Class*)>::type>
    promise<Result> finally(Method&& method, Class* obj) const
    {
      using task = internal::finally_class_task<Result, T, Method, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Method>(method), obj)};
    }


    /**
     * @brief Add a function to be called if the previous function was either resolved or rejected.
     * @param func - Function that not receives any result of the previous function.
     * @return Promise object.
     */
    template<typename Func, typename Result = typename std::result_of<Func()>::type>
    promise<Result> finally(Func&& func) const
    {
      using task = internal::finally_func_task<Result, T, Func>;
      return promise<Result>{std::make_shared<task>(m_task, std::forward<Func>(func))};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename Arg = T, typename FuncResult = typename std::result_of<Method(Class*, Arg)>::type,
             typename Result = Container<FuncResult, std::allocator<FuncResult>>,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_class_task<Result, Arg, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param methods - Methods that not receives any result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename FuncResult = typename std::result_of<Method(Class*)>::type,
             typename Result = Container<FuncResult, std::allocator<FuncResult>>,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_class_task_void<Result, T, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename Arg = T, typename FuncResult = typename std::result_of<Method(Class*, Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
    promise<void> all(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_class_task<void, Arg, Container, Method, Allocator, Class>;
      return promise<void>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param methods - Methods that not receives any result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename FuncResult = typename std::result_of<Method(Class*)>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
    promise<void> all(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_class_task_void<void, void, Container, Method, Allocator, Class>;
      return promise<void>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return either an iterable of results or the first rejection reason.
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
      using task = internal::all_func_task<Result, Arg, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename Result = Container<FuncResult, std::allocator<FuncResult>>,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_func_task_void<Result, T, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename FuncResult = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
    promise<void> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_func_task<void, Arg, Container, Func, Allocator>;
      return promise<void>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return either an iterable of results or the first rejection reason.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
    promise<void> all(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_func_task_void<void, void, Container, Func, Allocator>;
      return promise<void>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename Arg = T, typename FuncResult = typename std::result_of<Method(Class*, Arg)>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all_settled(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_settled_class_task<Result, Arg, FuncResult, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename FuncResult = typename std::result_of<Method(Class*)>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type,
             typename = typename std::true_type::type>
    promise<Result> all_settled(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_settled_class_task_void<Result, T, void, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename Arg = T, typename FuncResult = typename std::result_of<Method(Class*, Arg)>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type,
             typename = typename std::enable_if<std::is_void<FuncResult>::value>::type,
             typename = typename std::true_type::type>
    promise<Result> all_settled(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_settled_class_task<Result, Arg, void, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename FuncResult = typename std::result_of<Method(Class*)>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all_settled(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::all_settled_class_task_void<Result, T, FuncResult, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
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
      using task = internal::all_settled_func_task<Result, Arg, FuncResult, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
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
      using task = internal::all_settled_func_task_void<Result, T, void, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
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
      using task = internal::all_settled_func_task<Result, Arg, void, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return an iterable of @ref settled objects with either a result or an error.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename FuncResult = typename std::result_of<Func()>::type,
             typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
             typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
    promise<Result> all_settled(Container<Func, Allocator> funcs) const
    {
      using task = internal::all_settled_func_task_void<Result, T, FuncResult, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return the first resolved result.
     * @param methods - Methods that receives the result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename Arg = T, typename Result = typename std::result_of<Method(Class*, Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type>
    promise<Result> any(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::any_class_task<Result, Arg, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of class methods to be called next.
     *        Return the first resolved result.
     * @param methods - Methods that not receives any result of the previous function.
     * @param obj - Object containing the required methods.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
             typename Result = typename std::result_of<Method(Class*)>::type>
    promise<Result> any(Container<Method, Allocator> methods, Class* obj) const
    {
      using task = internal::any_class_task_void<Result, T, Container, Method, Allocator, Class>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(methods), obj)};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return the first resolved result.
     * @param funcs - Functions that receives the result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Arg = T, typename Result = typename std::result_of<Func(Arg)>::type,
             typename = typename std::enable_if<!std::is_void<Arg>::value>::type>
    promise<Result> any(Container<Func, Allocator> funcs) const
    {
      using task = internal::any_func_task<Result, Arg, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return the first resolved result.
     * @param funcs - Functions that not receives any result of the previous function.
     * @return Promise object.
     */
    template<template<typename, typename> class Container, typename Func, typename Allocator,
             typename Result = typename std::result_of<Func()>::type>
    promise<Result> any(Container<Func, Allocator> funcs) const
    {
      using task = internal::any_func_task_void<Result, T, Container, Func, Allocator>;
      return promise<Result>{std::make_shared<task>(m_task, std::move(funcs))};
    }


    /**
     * @brief Add an iterable of functions to be called next.
     *        Return either the first resolved or rejected result.
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
     *        Return either the first resolved or rejected result.
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
      return std::async(std::launch::async, &promise<T>::run_impl, this, m_task);
    }

  private:
    T run_impl(internal::task_ptr<T> task) const
    {
      return task->run();
    }

    internal::task_ptr<T> m_task;
};


/**
 * @brief Make a promise object with an initial class method.
 * @param method - Method for call.
 * @param obj - Object containing the required method.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<typename Method, typename Class, typename... Args,
         typename Result = typename std::result_of<Method(Class*, Args...)>::type,
         typename = typename std::enable_if<internal::is_invocable<Method, Class, Args...>::value>::type>
static promise<Result> make_promise(Method&& method, Class* obj, Args&&... args)
{
  return promise<Result>{std::forward<Method>(method), obj, std::forward<Args>(args)...};
}


/**
 * @brief Make a promise object with an initial function.
 * @param func - Function for call.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<typename Func, typename... Args,
         typename Result = typename std::result_of<Func(Args...)>::type>
static promise<Result> make_promise(Func&& func, Args&&... args)
{
  return promise<Result>{std::forward<Func>(func), std::forward<Args>(args)...};
}


/**
 * @brief Make a promise with an iterable of class methods to be called.
 *        Return promise object with either an iterable of results or the first rejection reason.
 * @param methods - Methods.
 * @param obj - Object containing the required methods.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Method,
         typename Allocator, typename Class, typename... Args,
         typename FuncResult = typename std::result_of<Method(Class*, Args...)>::type,
         typename Result = Container<FuncResult, std::allocator<FuncResult>>,
         typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type,
         typename = typename std::enable_if<internal::is_invocable<Method, Class, Args...>::value>::type>
static promise<Result> make_promise_all(Container<Method, Allocator> methods, Class* obj, Args&&... args)
{
  using task = internal::make_all_class_task<Result, Container, Method, Allocator, Class, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(methods), obj, std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of class methods to be called.
 *        Return promise object with either a void value or the first rejection reason.
 * @param methods - Methods.
 * @param obj - Object containing the required methods.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Method,
         typename Allocator, typename Class, typename... Args,
         typename FuncResult = typename std::result_of<Method(Class*, Args...)>::type,
         typename = typename std::enable_if<std::is_void<FuncResult>::value>::type,
         typename = typename std::enable_if<internal::is_invocable<Method, Class, Args...>::value>::type>
static promise<void> make_promise_all(Container<Method, Allocator> methods, Class* obj, Args&&... args)
{
  using task = internal::make_all_class_task<void, Container, Method, Allocator, Class, Args...>;
  return promise<void>{std::make_shared<task>(std::move(methods), obj, std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of functions to be called.
 *        Return promise object with either an iterable of results or the first rejection reason.
 * @param funcs - Functions.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args,
         typename FuncResult = typename std::result_of<Func(Args...)>::type,
         typename Result = Container<FuncResult, std::allocator<FuncResult>>,
         typename = typename std::enable_if<!std::is_void<FuncResult>::value>::type>
static promise<Result> make_promise_all(Container<Func, Allocator> funcs, Args&&... args)
{
  using task = internal::make_all_func_task<Result, Container, Func, Allocator, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of functions to be called.
 *        Return promise object with either a void value or the first rejection reason.
 * @param funcs - Functions.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args,
         typename FuncResult = typename std::result_of<Func(Args...)>::type,
         typename = typename std::enable_if<std::is_void<FuncResult>::value>::type>
static promise<void> make_promise_all(Container<Func, Allocator> funcs, Args&&... args)
{
  using task = internal::make_all_func_task<void, Container, Func, Allocator, Args...>;
  return promise<void>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of class methods to be called.
 *        Return an iterable of @ref settled objects with either a result or an error.
 * @param methods - Methods.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Method, typename Allocator, typename Class,
         typename... Args, typename FuncResult = typename std::result_of<Method(Class*, Args...)>::type,
         typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>,
         typename = typename std::enable_if<internal::is_invocable<Method, Class, Args...>::value>::type>
static promise<Result> make_promise_all_settled(Container<Method, Allocator> methods, Class* obj, Args&&... args)
{
  using task = internal::make_all_settled_class_task<Result, FuncResult, Container, Method, Allocator, Class, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(methods), obj, std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of functions to be called.
 *        Return an iterable of @ref settled objects with either a result or an error.
 * @param funcs - Functions.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args,
         typename FuncResult = typename std::result_of<Func(Args...)>::type,
         typename Result = Container<settled<FuncResult>, std::allocator<settled<FuncResult>>>>
static promise<Result> make_promise_all_settled(Container<Func, Allocator> funcs, Args&&... args)
{
  using task = internal::make_all_settled_func_task<Result, FuncResult, Container, Func, Allocator, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of class methods to be called.
 *        Return the first resolved result.
 * @param methods - Methods.
 * @param obj - Object containing the required methods.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Method,
         typename Allocator, typename Class, typename... Args,
         typename Result = typename std::result_of<Method(Class*, Args...)>::type,
         typename = typename std::enable_if<internal::is_invocable<Method, Class, Args...>::value>::type>
static promise<Result> make_promise_any(Container<Method, Allocator> methods, Class* obj, Args&&... args)
{
  using task = internal::make_any_class_task<Result, Container, Method, Allocator, Class, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(methods), obj, std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of functions to be called.
 *        Return the first resolved result.
 * @param funcs - Functions.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args,
         typename Result = typename std::result_of<Func(Args...)>::type>
static promise<Result> make_promise_any(Container<Func, Allocator> funcs, Args&&... args)
{
  using task = internal::make_any_func_task<Result, Container, Func, Allocator, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with an iterable of functions to be called.
 *        Return either the first resolved or rejected result.
 * @param funcs - Functions.
 * @param args - Optional arguments.
 * @return Promise object.
 */
template<template<typename, typename> class Container, typename Func, typename Allocator, typename... Args,
         typename Result = typename std::result_of<Func(Args...)>::type>
static promise<Result> make_promise_race(Container<Func, Allocator> funcs, Args&&... args)
{
  using task = internal::make_race_task<Result, Container, Func, Allocator, Args...>;
  return promise<Result>{std::make_shared<task>(std::move(funcs), std::forward<Args>(args)...)};
}


/**
 * @brief Make a promise with a resolved state.
 * @param value - Any value.
 * @return Promise object.
 */
template<typename T>
static promise<T> make_resolved_promise(T&& value)
{
  using task = internal::make_resolved_task<T>;
  return promise<T>{std::make_shared<task>(std::forward<T>(value))};
}


/**
 * @brief Make a promise with a resolved state.
 * @return Promise object.
 */
static promise<void> make_resolved_promise()
{
  using task = internal::make_resolved_task<void>;
  return promise<void>{std::make_shared<task>()};
}


/**
 * @brief Make a promise with a rejected state.
 * @param error - Any error
 * @return Promise object.
 */
template<typename T, typename Error>
static promise<T> make_rejected_promise(Error&& error)
{
  using task = internal::make_rejected_task<T, Error>;
  return promise<T>{std::make_shared<task>(std::forward<Error>(error))};
}


/**
 * @brief Make a promise with a rejected state.
 * @param error - Any error
 * @return Promise object.
 */
template<typename Error>
static promise<void> make_rejected_promise(Error&& error)
{
  using task = internal::make_rejected_task<void, Error>;
  return promise<void>{std::make_shared<task>(std::forward<Error>(error))};
}

} // namespace async

#endif // ASYNC_PROMISE_H
