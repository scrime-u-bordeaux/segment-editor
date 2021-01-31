#include "Scene.hpp"
#include <SEGMent/StringUtils.hpp>

#include <QUrl>

#include <SEGMent/Model/Model.hpp>
#include <SEGMent/ImageCache.hpp>
#include <SEGMent/Visitors.hpp>
#include <wobjectimpl.h>

W_OBJECT_IMPL(SEGMent::SceneModel)
namespace SEGMent
{

SceneModel::SceneModel(Id<SceneModel> id, QObject* parent)
    : base_type{std::move(id), "Scene", parent}
{
}

const SceneModel::SceneType& SceneModel::sceneType() const MSVC_NOEXCEPT
{
  return m_sceneType;
}

void SceneModel::setSceneType(const SceneModel::SceneType& v) MSVC_NOEXCEPT
{
  if (m_sceneType != v)
  {
    m_sceneType = v;
    sceneTypeChanged(v);
  }
}

const Sound& SceneModel::ambience() const MSVC_NOEXCEPT
{
  return m_ambience;
}
Sound& SceneModel::ambience_ref() MSVC_NOEXCEPT
{
  return m_ambience;
}

void SceneModel::setAmbience(const Sound& v) MSVC_NOEXCEPT
{
  if (m_ambience != v)
  {
    m_ambience = v;
    ambienceChanged(v);
  }
}

LongText SceneModel::startText() const MSVC_NOEXCEPT
{
  return {m_startText};
}

void SceneModel::setStartText(const LongText& v) MSVC_NOEXCEPT
{
  if (m_startText != v)
  {
    m_startText = v;
    startTextChanged(v);
  }
}

const Image& SceneModel::image() const MSVC_NOEXCEPT
{
  return m_image;
}

void SceneModel::setImage(const Image& v) MSVC_NOEXCEPT
{
  if (m_image != v)
  {
    if (metadata().getLabel().isEmpty())
      metadata().setLabel(QUrl(v).fileName().split(".")[0]);
    m_image = v;
    imageChanged(v);
  }
}

const QRectF& SceneModel::rect() const MSVC_NOEXCEPT
{
  return m_rect;
}

void SceneModel::setRect(const QRectF& v) MSVC_NOEXCEPT
{
  if (m_rect != v)
  {
    m_rect = v;
    rectChanged(v);
  }
}

bool SceneModel::repeatText() const MSVC_NOEXCEPT
{
  return m_repeatText;
}

void SceneModel::setRepeatText(bool v) MSVC_NOEXCEPT
{
  if (m_repeatText != v)
  {
    m_repeatText = v;
    repeatTextChanged(v);
  }
}

bool SceneModel::sonar() const MSVC_NOEXCEPT
{
  return m_sonar;
}

void SceneModel::setSonar(bool v) MSVC_NOEXCEPT
{
  if (m_sonar != v)
  {
    m_sonar = v;
    sonarChanged(v);
  }
}

const Cues& SceneModel::cue() const MSVC_NOEXCEPT
{
  return m_cues;
}

void SceneModel::setCue(const Cues& v) MSVC_NOEXCEPT
{
  if (m_cues != v)
  {
    m_cues = v;
    cueChanged(v);
  }
}

JournalEntry SceneModel::journal() const MSVC_NOEXCEPT
{
  return {m_journal};
}

void SceneModel::setJournal(const JournalEntry& v) MSVC_NOEXCEPT
{
  if (m_journal != v)
  {
    m_journal = v;
    journalChanged(v);
  }
}

bool SceneModel::journalBlink() const MSVC_NOEXCEPT
{
  return m_journalBlink;
}

void SceneModel::setJournalBlink(bool v) MSVC_NOEXCEPT
{
  if (m_journalBlink != v)
  {
    m_journalBlink = v;
    journalBlinkChanged(v);
  }
}

const QString& SceneModel::cuesToRemove() const MSVC_NOEXCEPT
{
  return m_cuesToRemove;
}

void SceneModel::setCuesToRemove(const QString& v) MSVC_NOEXCEPT
{
  if (m_cuesToRemove != v)
  {
    m_cuesToRemove = v;
    cuesToRemoveChanged(v);
  }
}

} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::LongText& v)
{
  m_stream << v.txt;
}
template <>
void DataStreamWriter::write(SEGMent::LongText& v)
{
  m_stream >> v.txt;
}

template <>
void DataStreamReader::read(const SEGMent::JournalEntry& v)
{
  m_stream << v.txt;
}
template <>
void DataStreamWriter::write(SEGMent::JournalEntry& v)
{
  m_stream >> v.txt;
}

template <>
void DataStreamReader::read(const SEGMent::Cue& v)
{
    m_stream << v.key << v.cues;
}
template <>
void DataStreamWriter::write(SEGMent::Cue& v)
{
    m_stream >> v.key >> v.cues;
}

template <>
void JSONObjectReader::read(const SEGMent::Cue& v)
{
    obj["Key"] = v.key;
    QJsonArray values;
    for(auto val : v.cues) values.push_back(val);

    obj["Values"] = values;
}
template <>
void JSONObjectWriter::write(SEGMent::Cue& v)
{
    v.key = obj["Key"].toString();
    auto arr = obj["Values"].toArray();
    for(auto val : arr) v.cues.push_back(val.toString());
}

template <>
void DataStreamReader::read(const SEGMent::SceneModel& v)
{
  // Serialize objects, gifs, etc
  SEGMent::forEachCategoryInScene(v,
      [&] (auto& map) {
      m_stream << (int32_t)map.size();
      for (const auto& obj : map)
      {
        readFrom(obj);
      }
  });

  // Serialize scene properties
  m_stream << v.m_ambience << v.m_image << v.m_rect << v.m_sceneType
           << v.m_startText << v.m_repeatText << v.m_sonar << v.m_cues << v.m_journal
           << v.m_journalBlink << v.m_cuesToRemove;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::SceneModel& v)
{
  // Deserialize objects, gifs, etc
  SEGMent::forEachCategoryInScene(v,
      [&] (auto& map) {
        using entity_type = typename std::remove_reference_t<decltype(map)>::value_type;

        int32_t sz;
        m_stream >> sz;
        for (; sz-- > 0;)
        {
          auto obj = new entity_type{*this, &v};
          map.add(obj);
        }
  });

  // Deserialize scene properties
  m_stream >> v.m_ambience >> v.m_image >> v.m_rect >> v.m_sceneType
      >> v.m_startText >> v.m_repeatText >> v.m_sonar >> v.m_cues >> v.m_journal
      >> v.m_journalBlink >> v.m_cuesToRemove;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::SceneModel& v)
{
  using namespace SEGMent;

  // Serialize objects, gifs, etc
  obj["Objects"] = toJsonArray(v.objects);
  obj["Gifs"] = toJsonArray(v.gifs);
  obj["ClickAreas"] = toJsonArray(v.clickAreas);
  obj["BackClickAreas"] = toJsonArray(v.backClickAreas);
  obj["TextAreas"] = toJsonArray(v.textAreas);

  // Serialize scene properties
  obj["Ambience"] = toJsonObject(v.m_ambience);
  obj["Image"] = v.m_image.path;

  auto parent_doc = score::IDocument::documentFromObject(v);
  auto& cache = SEGMent::ImageCache::instance().cache(SEGMent::toLocalFile(v.m_image.path, parent_doc->context()));
  obj["ImageSize"] = QJsonArray{cache.full_size.width(), cache.full_size.height()};
  obj["Rect"] = toJsonValue(v.m_rect);
  obj["SceneType"] = (int)v.m_sceneType;
  obj["StartText"] = v.m_startText;
  obj["RepeatText"] = v.m_repeatText;
  obj["Sonar"] = v.m_sonar;
  obj["Cue"] = toJsonArray(v.m_cues);
  obj["Journal"] = v.m_journal;
  obj["JournalBlink"] = v.m_journalBlink;
  obj["CuesToRemove"] = semicolonStringToStringList(v.m_cuesToRemove);
}

template <>
void JSONObjectWriter::write(SEGMent::SceneModel& v)
{
  using namespace SEGMent;

  // Serialize objects, gifs, etc
  {
    const auto& objs = obj["Objects"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::ImageModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.objects.add(obj);
    }
  }
  {
    const auto& objs = obj["Gifs"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::GifModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.gifs.add(obj);
    }
  }
  {
    const auto& objs = obj["ClickAreas"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::ClickAreaModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.clickAreas.add(obj);
    }
  }
  {
    const auto& objs = obj["BackClickAreas"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::BackClickAreaModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.backClickAreas.add(obj);
    }
  }
  {
    const auto& objs = obj["TextAreas"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::TextAreaModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.textAreas.add(obj);
    }
  }

  // Deserialize scene properties
  v.m_ambience = fromJsonObject<SEGMent::Sound>(obj["Ambience"]);
  v.m_image.path = obj["Image"].toString();
  v.m_rect = fromJsonValue<QRectF>(obj["Rect"]);
  v.m_sceneType = (SEGMent::SceneModel::SceneType)obj["SceneType"].toInt();
  v.m_startText = obj["StartText"].toString();
  v.m_repeatText = obj["RepeatText"].toBool();
  v.m_sonar = obj["Sonar"].toBool();
  fromJsonArray(obj["Cue"].toArray(), v.m_cues);
  v.m_journal = obj["Journal"].toString();
  v.m_journalBlink = obj["JournalBlink"].toBool();
  v.m_cuesToRemove = stringListToSemicolonString(obj["CuesToRemove"].toArray());
}
