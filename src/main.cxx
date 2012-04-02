/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "calcbox.hxx"

#include <gtkmm/main.h>
#include <gtkmm/window.h>

class MainWindow : public Gtk::Window
{
protected:
	CalcBox calcbox;

public:
	MainWindow();
};

MainWindow::MainWindow()
{
	set_border_width(10);

	add(calcbox);

	show_all();
}

int main(int argc, char* argv[])
{
	Gtk::Main kit(argc, argv);

	MainWindow w;

	Gtk::Main::run(w);

	return 0;
}
