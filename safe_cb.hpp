#pragma once

#include <memory>
#include <functional>
#include <utility>

template <typename PARENT, typename FN>
class safe_cb { };

/// A wrapper for std::function that makes sure that we don't have dangling this
/// pointers by forcing users to also pass a shared_ptr.
template <typename TYPE, typename RET, typename... ARGS>
class safe_cb<TYPE, RET(ARGS...)> {

  std::shared_ptr<TYPE>       obj_;
  std::function<RET(ARGS...)> fn_;

public:

  template <typename... FN_ARGS>
  RET operator()(FN_ARGS&&... args) const
  {
    return fn_(std::forward<FN_ARGS>(args)...);
  }

  template <typename CLOSURE>
  safe_cb(std::shared_ptr<TYPE> const &obj, CLOSURE &&fn)
    : obj_(obj), fn_(std::forward<CLOSURE>(fn))
  {}
};

// EOF
