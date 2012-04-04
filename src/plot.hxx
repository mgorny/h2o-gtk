/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#pragma once

#ifndef _H2O_GTK_PLOT_HXX
#define _H2O_GTK_PLOT_HXX 1

#include <vector>

#include <h2o>

#include <glibmm/refptr.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/table.h>
#include <plotmm/curve.h>
#include <plotmm/plot.h>
#include <sigc++/signal.h>

enum PlotAxisQuantity
{
	pa_p,
	pa_T,
	pa_v,
	pa_u,
	pa_h,
	pa_s
};

typedef double (h2o::H2O::*PlotAxisProperty)() const;

class PlotAxisChoice : public Gtk::ComboBoxText
{
public:
	PlotAxisChoice(int def = 0);

	enum PlotAxisQuantity get_quantity();
};

class PlotAxesControl : public Gtk::HBox
{
	typedef sigc::signal<void, enum PlotAxisQuantity, enum PlotAxisQuantity>
		axes_changed_sig;

	void axis_changed();

protected:
	PlotAxisChoice x, y;
	Gtk::Label sep;

	axes_changed_sig axes_changed;

public:
	PlotAxesControl();

	axes_changed_sig signal_axes_changed(); // (x, y)
};

class SaturationCurve : public PlotMM::Curve
{
public:
	SaturationCurve();

	void replot(PlotAxisProperty x_prop, PlotAxisProperty y_prop);
};

class DataCurve : public PlotMM::Curve
{
public:
	DataCurve(const char* color);

	void replot(PlotAxisProperty x_prop, PlotAxisProperty y_prop,
			std::vector<h2o::H2O>& data);
};

class Plot : public PlotMM::Plot
{
	PlotAxisProperty x_prop, y_prop;
	std::vector<h2o::H2O> current_data, user_data;

protected:
	Glib::RefPtr<SaturationCurve> saturation_curve;
	Glib::RefPtr<DataCurve> data_curve, user_plot_curve;;

public:
	Plot();

	void update_axes(enum PlotAxisQuantity x, enum PlotAxisQuantity y);
	void plot_data(h2o::H2O data[], int len);

	void append_plot();
	void clear_plot();
};

class PlotBottomControlBox : public Gtk::Table
{
	typedef sigc::signal<void> button_sig;

protected:
	Gtk::Button add_to_plot, clear_plot;
	button_sig add_to_plot_sig, clear_plot_sig;

public:
	PlotBottomControlBox();

	button_sig signal_add_to_plot();
	button_sig signal_clear_plot();
};

class PlotBox : public Gtk::VBox
{
protected:
	PlotAxesControl axes_control;
	Plot plot;
	PlotBottomControlBox bottom_control;

public:
	PlotBox();

	void update_data_plot(h2o::H2O* data, int len);
};

#endif /*_H2O_GTK_PLOT_HXX*/
