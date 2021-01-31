#pragma once
#include <score/document/DocumentContext.hpp>

#include <SEGMent/Visitors.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/Transition.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Commands/Deletion.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>

#include <limits>

namespace SEGMent
{
/**
 * @brief Computes the minimal and maximal z of the elements of a scene
 */
class SceneZOrders
{
public:
  template <typename T>
  SceneZOrders(const SceneModel& scene, const T& self)
  {
      dispatchSceneChildren(
                  scene,
                  [&]  (const auto& obj) {
      {
        using Obj_T = std::remove_reference_t<std::remove_const_t<decltype(obj)>>;
        if constexpr (std::is_same_v<T, Obj_T>)
        {
          if (&obj == &self)
            return;
        }
        min = std::min(min, obj.z());
        max = std::max(max, obj.z());
        count++;
      }
    });
  }

  int min{INT_MAX}, max{0};
  int count = 0;
};

/**
 * @brief Action used to put an item at the front.
 */
struct MoveForwardVisitor
{
  const score::DocumentContext& context;

  void operator()(const SceneModel& obj) const {}
  void operator()(const TransitionModel& obj) const {}

  template <typename T>
  void operator()(const T& obj) const
  {
    using cmd = get_command_type(T, z);

    auto [min, max, count]
        = SceneZOrders{*safe_cast<SceneModel*>(obj.parent()), obj};
    if (count == 0)
      return;

    const auto z = std::min(obj.z() + 1, max + 1);
    CommandDispatcher<>{context.commandStack}.submitCommand(
        new cmd{obj, z});
  }
};

/**
 * @brief Action used to put an item at the back.
 */
struct MoveBackwardsVisitor
{
  const score::DocumentContext& context;

  void operator()(const SceneModel& obj) const {}
  void operator()(const TransitionModel& obj) const {}

  template <typename T>
  void operator()(const T& obj) const
  {
    using cmd = get_command_type(T, z);

    auto [min, max, count]
        = SceneZOrders{*safe_cast<SceneModel*>(obj.parent()), obj};
    if (count == 0)
      return;
    if (min <= 1)
      min = 1;

    const auto z = std::max(obj.z() - 1, min - 1);
    CommandDispatcher<>{context.commandStack}.submitCommand(
        new cmd{obj, z});
  }
};

}
