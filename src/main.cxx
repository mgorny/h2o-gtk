/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "calcbox.hxx"
#include "expansionbox.hxx"
#include "saturationbox.hxx"
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
	SaturationBox saturation_box;
	ExpansionBox expansion_box;

public:
	MainHBox();

	void page_switched(GtkNotebookPage* p, guint n);
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
	saturation_box.signal_data_changed().connect(
			sigc::mem_fun(plotbox, &PlotBox::update_data_plot));
	expansion_box.signal_data_changed().connect(
			sigc::mem_fun(plotbox, &PlotBox::update_data_plot));
#endif /*HAVE_PLOTMM*/

	notebook.set_scrollable();
	notebook.append_page(single_point_box, "Single point");
	notebook.append_page(saturation_box, "Saturation");
	notebook.append_page(expansion_box, "Expansion");

	notebook.signal_switch_page().connect(
			sigc::mem_fun(*this, &MainHBox::page_switched));

#ifdef HAVE_PLOTMM
	pack_start(notebook, Gtk::PACK_SHRINK);
	pack_start(plotbox);
#else /*!HAVE_PLOTMM*/
	pack_start(notebook);
#endif /*HAVE_PLOTMM*/
}

void MainHBox::page_switched(GtkNotebookPage* p, guint n)
{
	switch (n)
	{
		case 0:
			single_point_box.recalc();
			break;
		case 1:
			saturation_box.recalc();
			break;
		case 2:
			expansion_box.recalc();
			break;
	}
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
