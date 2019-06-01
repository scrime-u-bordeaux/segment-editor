#pragma once
#include <score/tools/Todo.hpp>

#include <QLabel>

namespace score
{
template <typename Property_T>
class ReactiveLabel : public QLabel
{
public:
  ReactiveLabel(const typename Property_T::model_type& model, QWidget* parent)
      : QLabel{(model.*Property_T::get())(), parent}
  {
    con(model, Property_T::notify(), this, &QLabel::setText);
  }
};
} // namespace score
