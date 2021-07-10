// Copyright (c) 2012-2019 University of Lyon and CNRS (France).
// All rights reserved.
//
// This file is part of MEPP2; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation; either version 3 of the License,
// or (at your option) any later version.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#pragma once


#if defined _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4267) // for VS-2015 // MANDATORY when PCL is ON !!!
#endif


#include <QTimer>
#include <QMainWindow>
#include <QLabel>

// Plugins
#include <QDir>
#include <QActionGroup>

#include <QMdiSubWindow>

#include <iostream>
#include <ctime>
#include <algorithm>

#ifndef Q_MOC_RUN // MT : very important to avoid the error : ' Parse error at
                  // "BOOST_JOIN" ' -> (qt4 pb with boost)
#include "Visualization/BaseWindowQt.h"
#include "Visualization/BaseAdapterVisuQt.h"
#include "Visualization/Helpers/QtHelpers.h"
#include "Visualization/Helpers/OSGDebug.hpp" // @todo to be removed
#include "Visualization/Plugins/PluginInterface.h"
#include "Visualization/Plugins/PluginDialog.hpp"
#endif

#include "ui_SimpleWindow.h"

#include <QDebug>
#include <QMenuBar>

//#include <QMdiArea>

// DirView
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

#include <boost/assert.hpp>

// Plugins
#include <QPluginLoader>
// Plugins

#if 0
#include "Visualization/Qt/SimpleLineEdit.h"
#include "Visualization/Qt/SimpleCheckBox.h"
#endif

namespace FEVV {

class SimpleViewer;

class MdiArea; // NEW

/**
 * \class SimpleWindow
 * \brief SimpleWindow is a specialization of QMainWindow.
 * This class the Main Window.
 *
 * @see testViewer.cpp
 */
class SimpleWindow : public BaseWindowQt
{
  Q_OBJECT

public:
  /**
   * Constructor.
   *
   * @param[in]   _parent   Pointer to the QWidget parent of this QWidget (used
   * by Qt) (Default value = 0).
   * @param[in]   _flags    Windows flags (used by Qt) (Default value = 0).
   */
  SimpleWindow(QWidget *_parent = 0, Qt::WindowFlags _flags = 0);

  /**
   * Destructor.
   */
  ~SimpleWindow();

  /**
   * Attach a visualization adapter to the current Window.
   *
   * @param[in]   _adapter   Pointer to an adapter (with the scene, see
   * BaseAdapterVisu).
   */
  void attach(Adapter *_adapter) override;

  /**
   * Attach a visualization adapter to the current Window.
   *
   * @todo Need to check if we initialize with this function before other
   * operations.
   *
   * @param[in]   _adapter   Pointer to a Qt adapter (with the scene, see
   * BaseAdapterVisuQt).
   * @param[in]   _useMdiWindows  display MDI window instead of fullscreen
   * widgets (Default value = false).
   */
  void attach(AdapterQt *_adapter, const bool _useMdiWindows = false);

  void attachPlugin(Plugin *_plugin) override;

  void init() override;

  /**
   * initialize the window.
   *
   * @todo Need to check if we initialize with this function before other
   * operations.
   *
   * @param[in]   _test    boolean to enable test mode (will close the window
   * after 10 sec. This mode is useful for Travis/AppVeyor) (Default value =
   * false).
   * @param[in]   _width   Minimum width of the window  (Default value = 1024).
   * @param[in]   _height  Minimum heigth of the window (Default value =  768).
   */
  void init(const bool _test, const int _width = 1024, const int _height = 768);

  void notify() override;

  void setParam(std::string _name,
                int *_value,
                std::string _pluginName,
                Plugin *_plugin) override;

  void setParam(std::string _name,
                double *_value,
                std::string _pluginName,
                Plugin *_plugin) override;

  void setParam(std::string _name,
                float *_value,
                std::string _pluginName,
                Plugin *_plugin) override;

  void setParam(std::string _name,
                bool *_value,
                std::string _pluginName,
                Plugin *_plugin) override;

  void setParam(std::string _name,
                std::string *_value,
                std::string _pluginName,
                Plugin *_plugin) override;

  std::vector< Adapter * > getSelectedAdapters() override;
  std::vector< Adapter::Viewer * > getSelectedViewers() override;

  void loadQtPlugins();

  /*Q*/MdiArea *getMdiArea() { return mdiArea; }

  void sortModelList();

  void update(bool pick = false);

  void enableSpaceTimeMenus();

  void updateActiveChildTitle();

  // TODO : standardize template MeshT/HalfedgeGraph
  template< typename MeshT >
  void draw_or_redraw_mesh(/*const */ MeshT *mesh,
                           /*const */ FEVV::PMapsContainer *pmaps_bag,
                           FEVV::SimpleViewer *viewer,
                           bool _redraw = false,
                           bool _recomputeNT_if_redraw = false,
                           std::string _mesh_filename = std::string(""),
                           bool _recreateOSGobj_if_redraw = true,
                           float _step = 0.);

  void activate_time_mode();
  void activate_space_mode();

  void writeHG(FEVV::SimpleViewer *viewer);

  void actionHG(FEVV::SimpleViewer *viewer, char t, char t2);

  void centerHG(FEVV::SimpleViewer *viewer);

  QWidget *activeMdiChild();

#if 0 //TODO-elo-rm-?-ask_MTO
  template< typename HalfedgeGraph >
  void showSelectedHG(FEVV::SimpleViewer *viewer);
#endif

protected:
  void updateModelList(bool pick);

  // Plugins
  void populateMenus(QObject *plugin);
  void addToMenu(QObject *plugin,
                 const QStringList &texts,
                 QMenu *menu,
                 const char *member,
                 QActionGroup *actionGroup = 0);
  // Plugins

  void closeEvent(QCloseEvent *event) override;

  /**
   * \brief  Open button/menu callback function.
   */
  template< typename HalfedgeGraph >
  void on_actionOpen_SPACE_TIME(FEVV::SimpleViewer *viewer);

  /**
   * \brief  Create a new empty viewer.
   */
  FEVV::SimpleViewer* createNewViewer(void);

  /*!
   * \fn readSettings()
   * \brief Read settings: folder locations (tree/open/save), mainwindow (pos, size, state), recent files list... (init).
   */
  void readSettings();

  /*!
   * \fn writeSettings()
   * \brief Write settings: folder locations (tree/open/save), mainwindow (pos, size, state), recent files list... (exit).
   */
  void writeSettings();

public:
  /**
   * \brief  Open meshes in a viewer with datastructure HalfedgeGraph.
   *
   * \param  viewer  viewer to open the meshes into ; if nullptr is passed,
   *                 a new viewer is opened
   * \param  filenames  list of mesh file names to open
   */
  template< typename HalfedgeGraph >
  void open_SPACE_TIME(FEVV::SimpleViewer *viewer,
                       const std::vector< std::string >& filenames);


protected slots:
  void on_actionNew_triggered();
  //void on_actionOpen_triggered();
  void on_actionSaveAs_triggered();
  void on_actionClose_triggered();
  void on_actionQuit_triggered();

  void on_actionClose_window_triggered();
  void on_actionClose_all_triggered();
  void on_actionChange_MDI_view_mode_triggered();
  void on_actionChange_viewer_mode_triggered();

  void on_actionAbout_MEPP_Help_triggered();
  void onGrab();

  void pre_actionHG(Adapter::Viewer *viewer, char t = 'D', char t2 = '_');

  void on_actionRender_Point_triggered();
  void on_actionRender_Line_triggered();
  void on_actionRender_Fill_triggered();
  void on_actionSuperimpose_Vertices_triggered();
  void on_actionSuperimpose_Vertices_bigger_triggered();
  void on_actionSuperimpose_Edges_triggered();
  void on_actionVertex_Color_triggered();
  void on_actionFace_Color_triggered();
  void on_actionTexture_Mode_triggered();
  void on_actionLighting_triggered();
  void on_actionSmoothFlat_Shading_triggered();
  void on_actionRender_Mode_triggered();

  void on_actionShow_Entire_Mesh_triggered();
  void on_actionCopy_Viewpoint_triggered();
  void on_actionPaste_Viewpoint_triggered();

  void on_actionShow_Axis_triggered();
  void on_actionShow_Grid_triggered();
  void on_actionShow_Vertex_Normals_triggered();
  void on_actionShow_CustomVectors_Vertices_triggered();
#if 0 //TODO-elo-rm-?-ask_MTO
  void on_actionShow_Selected_triggered();
#endif
  void on_actionShow_Translation_Draggers_triggered();
  void on_actionShow_Rotation_Draggers_triggered();

  void on_actionDyn_First_triggered();
  void on_actionDyn_Previous_triggered();
  void on_actionDyn_Next_triggered();
  void on_actionDyn_Last_triggered();

  void onAddBall();

  void onItemChanged(QListWidgetItem *_item, QListWidgetItem *_item_old);

  // Plugins
  void applyPlugin();
  void aboutPlugins();
  // Plugins

  /*!
   * \fn openRecentFile()
   * \brief Open a recent file (from menu).
   */
  void openRecentFile();

public slots:
  void onModificationParam(std::string _pluginName, BasePlugin *_plugin);
  void onApplyButton();

  void on_actionOpen_triggered();

protected:
  QTimer timerQuit;

  // Plugins
  QDir pluginsDir;
  QStringList pluginFileNames;
  QMenu *menuPlugins;

  /*Q*/MdiArea *mdiArea = nullptr;

  // DirView
  QDockWidget *dockDirView;
  QFileSystemModel *model;
  QSortFilterProxyModel *proxyModel;
  QTreeView *tree;

  QString treeLocation, openLocation, saveLocation;
  int m_dockDirView_MinimumWidth;

  // ---

  /*!
   * \fn setCurrentFile()
   * \brief Set a mesh filename in recent files list.
   *
   * \param fileName filename of the mesh.
   */
  void setCurrentFile(const QString &fileName);
  /*!
   * \fn strippedName(const QString &fullFileName)
   * \brief Return the filename of a mesh without path.
   *
   * \param fullFileName filename of the mesh.
   * \return QString.
   */
  QString strippedName(const QString &fullFileName);

  /*!
   * \fn updateRecentFileActions()
   * \brief Update Mepp recent files list.
   */
  void updateRecentFileActions();

  QAction *separatorAct_menuFile; //!< separator in file menu
  enum { MaxRecentFiles = 9 }; //!< only 9 mesh files in recent list
  QAction *recentFileActs[MaxRecentFiles]; //!< tab of mesh files recent list

  // ---

  bool useMdiWindows = false;

  bool open_only_pts_mode = false;

  bool ctrl_pressed = false;

  Ui::MainWindow ui; //<! Interface : QtDesigner

public:
  QStringList drag_files;

  bool drag = false;
  bool shift_drag = false;
  bool alt_drag = false;
  bool ctrl_drag = false;

  bool recent = false;

private:
  bool clean_closure = false;

  osg::Matrix matrixVP;
};

} // namespace FEVV

#ifndef Q_MOC_RUN
#include "Visualization/SimpleWindow.inl"
#endif // Q_MOC_RUN


#if defined _MSC_VER
#pragma warning(pop)
#endif
