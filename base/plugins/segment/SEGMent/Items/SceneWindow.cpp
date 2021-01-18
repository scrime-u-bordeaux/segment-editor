#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/selection/SelectionStack.hpp>

#include <QBuffer>
#include <QFileInfo>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsView>
#include <QJsonDocument>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QStyleOption>

#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Commands/Deletion.hpp>
#include <SEGMent/Commands/Properties.hpp>
#include <SEGMent/FilePath.hpp>
#include <SEGMent/Items/ClickWindow.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/ImageCache.hpp>
#include <score/widgets/SignalUtils.hpp>

namespace SEGMent
{

class SceneResizer final : public SizeGripItem::Resizer
{
public:
  SceneResizer(SceneWindow& w) : self{w} {}
  SceneWindow& self;
  void start(QGraphicsItem* item) override {}

  void operator()(QGraphicsItem* item, QRectF& rect) override
  {
    auto new_rect
        = QRectF{self.pos().x(),
                 self.pos().y(),
                 rect.width(),
                 self.rect().height() / self.rect().width() * rect.width()};
    auto rectItem = static_cast<SceneWindow*>(item);
    self.context.dispatcher.submitCommand<SEGMent::SetSceneRect>(
        self.model(), new_rect);
    rect = rectItem->rect();
  }

  void finish(QGraphicsItem* item) override
  {
    self.context.dispatcher.commit();
  }
};
SceneWindow::SceneWindow(
    const SceneModel& p,
    const score::DocumentContext& ctx,
    ZoomView& view,
    QGraphicsItem* parent)
    : Window(
        QPointF{p.rect().x(), p.rect().y()},
        QSizeF{p.rect().width() + (2 * borderWidth),
               p.rect().height() + titleBarHeight + (2 * borderWidth)},
        false,
        true,
        ctx,
        view,
        parent)
    , m_scene{p}
    , m_view(view)
    , m_titleBar{0,
                 0,
                 p.rect().width(),
                 titleBarHeight,
                 false,
                 ctx,
                 view,
                 this}
    , m_titleProxy{&m_titleBar}
    , m_title{m_titleProxy.widget()}
    , m_sceneArea{0,
                  0,
                  p.rect().width(),
                  p.rect().height(),
                  false,
                  ctx,
                  view,
                  this}
{
  m_titleBar.setFlag(ItemHasNoContents, false);
  setMinSize(200, 200);

  static const QColor initial_color = QColor::fromRgb(45, 131, 76);
  static const QColor default_color = QColor::fromRgb(67, 121, 131);
  static const QColor final_color = QColor::fromRgb(131, 67, 45);
  static const QColor gameover_color = QColor::fromRgb(131, 67, 45);
  const auto set_titlebar_color = [&](const SceneModel::SceneType t) {
    switch (t)
    {
      case SceneModel::SceneType::Default:
        m_titleBar.setBrush(default_color);
        break;
      case SceneModel::SceneType::Initial:
        m_titleBar.setBrush(initial_color);
        break;
      case SceneModel::SceneType::Final:
        m_titleBar.setBrush(final_color);
        break;
      case SceneModel::SceneType::GameOver:
        m_titleBar.setBrush(gameover_color);
        break;
    }
  };

  set_titlebar_color(p.sceneType());
  con(m_scene, &SceneModel::sceneTypeChanged, this, set_titlebar_color);

  m_titleBar.moveBy(borderWidth, borderWidth);
  m_titleBar.setAcceptDrops(false);
  m_titleBar.setFlag(GraphicsItemFlag::ItemClipsChildrenToShape, false);
  m_titleBar.setFlag(GraphicsItemFlag::ItemClipsToShape, false);

  m_titleProxy.setWidget(&m_title);
  m_titleBar.setFlag(GraphicsItemFlag::ItemClipsChildrenToShape, false);
  m_titleBar.setFlag(GraphicsItemFlag::ItemClipsToShape, false);

  m_title.setAlignment(Qt::AlignHCenter);
  m_title.setStyleSheet(Style::instance().sceneLabelStyle);
  m_title.setAttribute(Qt::WA_TransparentForMouseEvents);

  con(m_title, &QLineEdit::editingFinished, this, [&] {
    CommandDispatcher<> disp{context.commandStack};
    disp.submitCommand(new SetSceneLabel{m_scene, m_title.text()});
  });

  m_backgroundImgDisplay.setParentItem(&m_sceneArea);

  m_sceneArea.moveBy(borderWidth, titleBarHeight + borderWidth);

  m_sizeGripItem = new SizeGripItem(new SceneResizer{*this}, this, true);

  this->setAcceptDrops(true);
  this->setFlag(ItemSendsGeometryChanges);
  this->setFlag(ItemIsSelectable);
  this->setFlag(ItemHasNoContents, false);

  QObject::connect(
      this, &RectItem::on_sizeChanged, this, &SceneWindow::sizeChanged);

  con(p.selection, &Selectable::changed, this, [=](bool b) {
    m_anchorsSetter.setVisible(b);
    if(!b) {
      setSelected(false);
    }
  }, Qt::QueuedConnection);


  ossia::for_each_in_tuple(m_items, [this] (auto& item) { item.init(*this); });

  ::bind(p, SceneModel::p_image{}, this, [=, &ctx](const Image& img) {
    setBackgroundImage(ImageCache::instance().cache(toLocalFile(img.path, ctx)));
  });
  ::bind(p.metadata(), score::ModelMetadata::p_label{}, this, [=](auto img) {
    setTitle(img);
  });
  ::bind(p, SceneModel::p_rect{}, this, [=](auto rect) {
    setRect({0, 0, rect.width(), rect.height()});
    setPos({rect.x(), rect.y()});
  });
}

SceneWindow::~SceneWindow()
{
  m_titleProxy.setWidget(nullptr);
}


void SceneWindow::setTitle(QString title)
{
  m_title.setText(title);
}

void SceneWindow::sizeChanged()
{
  m_sceneArea.setRect(
      m_sceneArea.rect().x(),
      m_sceneArea.rect().y(),
      this->rect().width() - borderWidth * 2,
      m_sceneArea.rect().height());

  m_backgroundImgDisplay.setScale(
      m_sceneArea.rect().width() / (m_backgroundImgRealWidth));

  m_sceneArea.setRect(
      m_sceneArea.rect().x(),
      m_sceneArea.rect().y(),
      m_backgroundImgDisplay.boundingRect().width()
          * m_backgroundImgDisplay.scale(),
      m_backgroundImgDisplay.boundingRect().height()
          * m_backgroundImgDisplay.scale());

  m_titleBar.setRect(
      m_titleBar.rect().x(),
      m_titleBar.rect().y(),
      m_sceneArea.rect().width(),
      m_titleBar.rect().height());

  m_title.FitToTextAndCenterToParent();

  QObject::disconnect(
      this, &RectItem::on_sizeChanged, this, &SceneWindow::sizeChanged);

  this->setRect(
      this->rect().x(),
      this->rect().y(),
      m_sceneArea.rect().width() + 2 * borderWidth,
      m_titleBar.rect().height() + m_sceneArea.rect().height()
          + 2 * borderWidth);
  m_anchorsSetter.updateAnchorsPos();

  if (m_sizeGripItem)
    m_sizeGripItem->reset();

  QObject::connect(
      this, &RectItem::on_sizeChanged, this, &SceneWindow::sizeChanged);

  const auto w = this->rect().width();
  const auto h = this->rect().height();

  m_topLine.setLength(w );
  m_bottomLine.setLength(w );
  m_bottomLine.setPos(0, h );
  m_leftLine.setHeight(h );
  m_rightLine.setHeight(h );
  m_rightLine.setPos(w , 0);
}

void SceneWindow::setBackgroundImage(CacheInstance& img)
{
  //m_scene.image().cache = img;
  m_backgroundImgDisplay.setPixmap(img);
  m_backgroundImgRealWidth = img.full_size.width();
  if(m_backgroundImgRealWidth < 1)
      m_backgroundImgRealWidth = img.fullPixmap().width();

  const auto sceneRect = m_sceneArea.boundingRect();

  m_backgroundImgDisplay.setPos(sceneRect.x(), sceneRect.y());

  qreal fitRatio = sceneRect.width() / m_backgroundImgRealWidth;

  m_backgroundImgDisplay.setScale(fitRatio);

  m_sceneArea.setRect(
      m_backgroundImgDisplay.pos().x(),
      m_backgroundImgDisplay.pos().y(),
      m_backgroundImgDisplay.boundingRect().width() * fitRatio,
      m_backgroundImgDisplay.boundingRect().height() * fitRatio);

  const qreal w = m_sceneArea.rect().width() + 2 * borderWidth;
  const qreal h = m_titleBar.rect().height() + m_sceneArea.rect().height() + 2 * borderWidth;
  this->setRect(
      this->rect().x(),
      this->rect().y(),
      w,
      h);

  m_anchorsSetter.updateAnchorsPos();

  if (m_sizeGripItem)
    m_sizeGripItem->reset();
}

void SceneWindow::updateRect() {}

void SceneWindow::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  const QMimeData* currentMimeData = event->mimeData();

  QPointF localPos = this->mapFromScene(event->scenePos());
  QSizeF sz{1. / 5., 1. / 5.};
  QPointF pos{localPos.x() / this->boundingRect().width(),
              localPos.y() / this->boundingRect().height()};

  if (currentMimeData->hasUrls())
  {
    if (currentMimeData->text() == SEGMENT_OBJECT_ID)
    {
      CommandDispatcher<> disp{context.commandStack};

      QUrl currentUrl = currentMimeData->urls().first();
      auto file = toLocalFile(currentUrl.path(), context);
      QImage img{file};
      sz = {1. / 5., img.height() / (img.width() * 5.)};
      if (pos.x() + sz.width() > 0.9)
        pos.setX(0.9 - sz.width());

      if (pos.y() + sz.height() > 0.9)
        pos.setY(0.9 - sz.height());

      if (QFileInfo(currentUrl.fileName())
              .completeSuffix()
              .compare("gif", Qt::CaseInsensitive)
          == 0)
      {
        disp.submitCommand(
            new DropGif{m_scene, Image{currentUrl.path()}, pos, sz});
      }
      else
      {
        disp.submitCommand(
            new DropImage{m_scene, Image{currentUrl.path()}, pos, sz});
      }
    }
    else if (currentMimeData->text() == SEGMENT_SCENE_ID)
    {
      QUrl currentUrl = currentMimeData->urls().first();

      CommandDispatcher<> disp{context.commandStack};
      disp.submitCommand(new SetSceneImage{m_scene, currentUrl.path()});
    }
    else if (currentMimeData->text() == SEGMENT_SOUND_ID)
    {
      QUrl currentUrl = currentMimeData->urls().first();

      CommandDispatcher<> disp{context.commandStack};
      disp.submitCommand(new SetSceneAmbience{m_scene, currentUrl.path()});
    }
  }

  {
    auto data = currentMimeData->data("segment/drag");
    if (data == SEGMENT_CLICKAREA_ID)
    {
      CommandDispatcher<> disp{context.commandStack};

      disp.submitCommand(new DropClickArea{m_scene, pos, sz});
    }
    else if (data == SEGMENT_BACKCLICKAREA_ID)
    {
      CommandDispatcher<> disp{context.commandStack};

      disp.submitCommand(new DropBackClickArea{m_scene, pos, sz});
    }
    else if (data == SEGMENT_TEXTAREA_ID)
    {
      CommandDispatcher<> disp{context.commandStack};

      disp.submitCommand(new DropTextArea{m_scene, pos, sz});
    }
  }

  event->accept();
  return;
}

constexpr qreal max_dim = 20000.;
QVariant
SceneWindow::itemChange(GraphicsItemChange change, const QVariant& value)
{
  switch (change)
  {
    case GraphicsItemChange::ItemPositionChange:
    {
      QPointF pt = value.toPointF();
      pt.setX(qBound(-max_dim, pt.x(), max_dim));
      pt.setY(qBound(-max_dim, pt.y(), max_dim));
      return pt;
    }
    case GraphicsItemChange::ItemSelectedChange:
    {
      if(value.toUInt())
      {
        const auto& sel = context.selectionStack.currentSelection();
        if(ossia::any_of(sel, [] (const auto& p){ return !dynamic_cast<const SceneModel*>(p.data()); }))
        {
          context.selectionStack.pushNewSelection({&m_scene});
        }
        else
        {
          context.selectionStack.addToSelection(&m_scene);
        }
      }
      else
      {
        context.selectionStack.removeFromSelection(&m_scene);
      }
      return value;
    }
    default:
      return value;
  }
}

void renderGraphicsItemRec(
    QPainter& painter,
    QGraphicsItem& root,
    QGraphicsItem& item,
    QWidget* view)
{
  if (!dynamic_cast<Anchor*>(&item))
  {
    QStyleOptionGraphicsItem opt;

    painter.translate(item.mapToItem(&root, QPointF{}));
    painter.scale(item.scale(), item.scale());

    item.paint(&painter, &opt, view);
    painter.resetTransform();
  }

  for (auto cld : item.childItems())
    renderGraphicsItemRec(painter, root, *cld, view);
}

QImage renderGraphicsItem(QGraphicsItem& item, QWidget* view)
{
  QImage p(
      item.boundingRect().width(),
      item.boundingRect().height(),
      QImage::Format::Format_ARGB32);
  p.fill(Qt::white);
  {
    QPainter painter{&p};
    QStyleOptionGraphicsItem opt;
    item.paint(&painter, &opt, view);

    for (auto cld : item.childItems())
      renderGraphicsItemRec(painter, item, *cld, view);
  }
  return p;
}

QString imageToBase64(const QImage& image)
{
  QByteArray byteArray;
  QBuffer buffer(&byteArray);
  image.save(&buffer, "PNG");
  return QString::fromLatin1(
      byteArray.toBase64(QByteArray::Base64Encoding).data());
}


void SceneWindow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    QGraphicsRectItem::mousePressEvent(event);
  }
  event->accept();
}

void SceneWindow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if(event->buttons() & Qt::LeftButton)
  {
    QGraphicsRectItem::mouseMoveEvent(event);
  }
  event->accept();
}

void SceneWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if(event->button() == Qt::LeftButton)
  {
    QGraphicsRectItem::mouseReleaseEvent(event);
    {
      std::vector<const SceneModel*> scenes;

      for(auto& obj : context.selectionStack.currentSelection())
      {
        if(auto scene = dynamic_cast<const SceneModel*>(obj.data()))
        {
          scenes.push_back(scene);
        }
      }

      if(!scenes.empty())
      {
        QPointF translation = event->scenePos() - event->buttonDownScenePos(Qt::LeftButton);
        auto aggr = new MoveSceneRects;
        for(auto scene : scenes)
        {
          auto rect = scene->rect();
          if(scene != &m_scene)
          {
            double x = qBound(-max_dim, rect.x() + translation.x(), max_dim);
            double y = qBound(-max_dim, rect.y() + translation.y(), max_dim);

            aggr->addCommand(new SetSceneRect{*scene, QRectF{x, y, rect.width(), rect.height()}});
          }
          else
          {
            double x = qBound(-max_dim, this->pos().x(), max_dim);
            double y = qBound(-max_dim, this->pos().y(), max_dim);

            aggr->addCommand(new SetSceneRect{*scene, QRectF{x, y, rect.width(), rect.height()}});
          }
        }

        context.commandStack.redoAndPush(aggr);
      }
    }
  }

  if (event->button() == Qt::RightButton)
  {
    auto m = new QMenu;
    {
      auto act = m->addAction(tr("Delete"));
      connect(act, &QAction::triggered, [=] {
        CommandDispatcher<> c{this->context.commandStack};
        c.submitCommand(new RemoveScene{
            (const SEGMent::ProcessModel&)*m_scene.parent(), m_scene});
      });
    }

    {
      auto act = m->addAction(tr("Save template"));
      connect(act, &QAction::triggered, [=] {
        // Save the scene
        auto json_object = score::marshall<JSONObject>(m_scene);

        // Generate a pixmap
        json_object["Pixmap"] = imageToBase64(
            renderGraphicsItem(*this, this->scene()->views().first())
                .scaled(100, 100));

        // Save in a file in the library
        auto ressources = ressourcesFolder(context);
        auto template_file = addUniqueSuffix(
            ressources + "/Templates/" + m_scene.metadata().getLabel()
            + ".template");
        QDir{ressources}.mkpath("Templates/");
        QFile f(template_file);
        if (f.open(QFile::WriteOnly))
        {
          f.write(QJsonDocument{json_object}.toJson());
        }
      });
    }
    m->exec(event->screenPos());
    m->deleteLater();
  }

  event->accept();
}

transition_t SceneWindow::createTransitionFrom(
    anchor_id source_anchor,
    const SceneModel& target,
    anchor_id target_anchor)
{
  return SceneToScene{model(), target, source_anchor, target_anchor};
}

} // namespace SEGMent
