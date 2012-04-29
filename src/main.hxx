/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_MAIN_HXX
#define _H2O_GTK_MAIN_HXX 1

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/window.h>

#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menuitem.h>

#include "calcbox.hxx"
#include "expansionbox.hxx"
#include "saturationbox.hxx"
#include "plot.hxx"

class MainHBox : public Gtk::HBox
{
protected:
#ifdef HAVE_PLOTMM
	PlotBox plotbox;
#endif /*HAVE_PLOTMM*/
	Gtk::Notebook notebook;

	CalcBox single_point_box;
	SaturationBox saturation_box;
	ExpansionBox expansion_box;

public:
	MainHBox();

	void page_switched(GtkNotebookPage* p, guint n);
};

class QuitItem : public Gtk::ImageMenuItem
{
protected:
	void on_activate();

public:
	QuitItem();
};

class MainMenu : public Gtk::MenuBar
{
protected:
	Gtk::Menu file_menu;
	Gtk::ImageMenuItem file;

public:
	MainMenu();
};

class MainWindow : public Gtk::Window
{
protected:
	Gtk::VBox all;

	MainMenu menu;
	MainHBox cont;

public:
	MainWindow();
};

#endif /*_H2O_GTK_MAIN_HXX*/
