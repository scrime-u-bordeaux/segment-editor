#pragma once
#include <score/model/Entity.hpp>
#include <score/model/EntityMap.hpp>
#include <score/selection/Selectable.hpp>

#include <SEGMent/Model/BackClickArea.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/SimpleObject.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/TextArea.hpp>
namespace SEGMent
{

struct LongText
{
  QString txt;
  operator const QString&() const noexcept { return txt; }
  operator QString&() noexcept { return txt; }
  friend bool operator==(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt == rhs.txt;
  }
  friend bool operator!=(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt != rhs.txt;
  }
  friend bool operator<(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt < rhs.txt;
  }
  friend bool operator>(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt > rhs.txt;
  }
  friend bool operator<=(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt <= rhs.txt;
  }
  friend bool operator>=(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt >= rhs.txt;
  }
};

//! A scene is the main object in a SEGMent canvas
class SceneModel : public PathAsId<score::Entity<SceneModel>>
{
  W_OBJECT(SceneModel)
  SCORE_SERIALIZE_FRIENDS
public:
  Selectable selection{this};

  using base_type = PathAsId<score::Entity<SceneModel>>;

  SceneModel(Id<SceneModel> id, QObject* parent);

  template <typename DeserializerVisitor>
  SceneModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{std::forward<DeserializerVisitor>(vis), parent}
  {
    vis.writeTo(*this);
  }

  enum SceneType
  {
    Default,
    Initial,
    Final,
    GameOver
  };

  score::EntityMap<ImageModel> objects;
  score::EntityMap<GifModel> gifs;
  score::EntityMap<ClickAreaModel> clickAreas;
  score::EntityMap<BackClickAreaModel> backClickAreas;
  score::EntityMap<TextAreaModel> textAreas;

public:
  const SceneType& sceneType() const MSVC_NOEXCEPT;
  void setSceneType(const SceneType& v) MSVC_NOEXCEPT;
  void sceneTypeChanged(const SceneType& v) W_SIGNAL(sceneTypeChanged, v);
  PROPERTY(
      SceneType,
      sceneType READ sceneType WRITE setSceneType NOTIFY sceneTypeChanged)

private:
  SceneType m_sceneType{SceneType::Default};

public:
  const Sound& ambience() const MSVC_NOEXCEPT;
  Sound& ambience_ref() MSVC_NOEXCEPT;
  void setAmbience(const Sound& v) MSVC_NOEXCEPT;
  void ambienceChanged(const Sound& v) W_SIGNAL(ambienceChanged, v);
  PROPERTY(
      Sound,
      ambience READ ambience WRITE setAmbience NOTIFY ambienceChanged)

private:
  Sound m_ambience;

public:
  LongText startText() const MSVC_NOEXCEPT;
  void setStartText(const LongText& v) MSVC_NOEXCEPT;
  void startTextChanged(LongText v) W_SIGNAL(startTextChanged, v);
  PROPERTY(
      LongText,
      startText READ startText WRITE setStartText NOTIFY startTextChanged)

private:
  QString m_startText;

public:
  const Image& image() const MSVC_NOEXCEPT;
  void setImage(const Image& v) MSVC_NOEXCEPT;
  void imageChanged(const Image& v) W_SIGNAL(imageChanged, v);
  PROPERTY(Image, image READ image WRITE setImage NOTIFY imageChanged)

private:
  Image m_image;

public:
  const QRectF& rect() const MSVC_NOEXCEPT;
  void setRect(const QRectF& v) MSVC_NOEXCEPT;
  void rectChanged(const QRectF& v) W_SIGNAL(rectChanged, v);
  PROPERTY(QRectF, rect READ rect WRITE setRect NOTIFY rectChanged)

private:
  QRectF m_rect{};

public:
  bool repeatText() const MSVC_NOEXCEPT;
  void setRepeatText(bool v) MSVC_NOEXCEPT;
  void repeatTextChanged(bool v) W_SIGNAL(repeatTextChanged, v);
  PROPERTY(
      bool,
      repeatText READ repeatText WRITE setRepeatText NOTIFY repeatTextChanged)

private:
  bool m_repeatText{};

public:
  bool sonar() const MSVC_NOEXCEPT;
  void setSonar(bool v) MSVC_NOEXCEPT;
  void sonarChanged(bool v) W_SIGNAL(sonarChanged, v);
  PROPERTY(bool, sonar READ sonar WRITE setSonar NOTIFY sonarChanged)

private:
  bool m_sonar{};

public:
  const QString& cue() const MSVC_NOEXCEPT;
  void setCue(const QString& v) MSVC_NOEXCEPT;
  void cueChanged(const QString& v) W_SIGNAL(cueChanged, v);
  PROPERTY(QString, cue READ cue WRITE setCue NOTIFY cueChanged)

private:
  QString m_cue{};
};

template <typename T>
struct SceneAccessor;
template <>
struct SceneAccessor<ImageModel>
{
  static auto& get(SceneModel& scene) { return scene.objects; }
  static auto& get(const SceneModel& scene) { return scene.objects; }
};
template <>
struct SceneAccessor<GifModel>
{
  static auto& get(SceneModel& scene) { return scene.gifs; }
  static auto& get(const SceneModel& scene) { return scene.gifs; }
};
template <>
struct SceneAccessor<ClickAreaModel>
{
  static auto& get(SceneModel& scene) { return scene.clickAreas; }
  static auto& get(const SceneModel& scene) { return scene.clickAreas; }
};
template <>
struct SceneAccessor<BackClickAreaModel>
{
  static auto& get(SceneModel& scene) { return scene.backClickAreas; }
  static auto& get(const SceneModel& scene) { return scene.backClickAreas; }
};
template <>
struct SceneAccessor<TextAreaModel>
{
  static auto& get(SceneModel& scene) { return scene.textAreas; }
  static auto& get(const SceneModel& scene) { return scene.textAreas; }
};
} // namespace SEGMent

DEFAULT_MODEL_METADATA(SEGMent::SceneModel, "Scene")
Q_DECLARE_METATYPE(SEGMent::SceneModel::SceneType)
W_REGISTER_ARGTYPE(SEGMent::SceneModel::SceneType)
Q_DECLARE_METATYPE(SEGMent::LongText)
W_REGISTER_ARGTYPE(SEGMent::LongText)
