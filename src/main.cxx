/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "calcbox.hxx"
#include "plot.hxx"

#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>

class MainHBox : public Gtk::HBox
{
protected:
	CalcBox calcbox;
	PlotBox plotbox;

public:
	MainHBox();
	virtual ~MainHBox();
};

class MainWindow : public Gtk::Window
{
protected:
	MainHBox cont;

public:
	MainWindow();
};

MainHBox::MainHBox()
{
	set_spacing(10);

	pack_start(calcbox, Gtk::PACK_SHRINK);
	pack_start(plotbox);
}

MainHBox::~MainHBox()
{
}

MainWindow::MainWindow()
{
	set_border_width(10);

	add(cont);

	show_all();
}

int main(int argc, char* argv[])
{
	Gtk::Main kit(argc, argv);

	MainWindow w;

	Gtk::Main::run(w);

	return 0;
}
