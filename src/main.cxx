/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "calcbox.hxx"
#include "plot.hxx"

#include <h2o>

#include <gtkmm/box.h>
#include <gtkmm/main.h>
#include <gtkmm/notebook.h>
#include <gtkmm/window.h>
#include <sigc++/functors/mem_fun.h>

class MainHBox : public Gtk::HBox
{
protected:
#ifdef HAVE_PLOTMM
	PlotBox plotbox;
#endif /*HAVE_PLOTMM*/
	Gtk::Notebook notebook;

	CalcBox single_point_box;

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

#ifdef HAVE_PLOTMM
	single_point_box.signal_data_changed().connect(
			sigc::mem_fun(plotbox, &PlotBox::update_data_plot));
#endif /*HAVE_PLOTMM*/

	single_point_box.recalc();

	notebook.append_page(single_point_box, "Single point");

#ifdef HAVE_PLOTMM
	pack_start(notebook, Gtk::PACK_SHRINK);
	pack_start(plotbox);
#else /*!HAVE_PLOTMM*/
	pack_start(notebook);
#endif /*HAVE_PLOTMM*/
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
