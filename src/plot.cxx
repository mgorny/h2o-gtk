/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "plot.hxx"

#include <cassert>

#include <gdkmm/color.h>
#include <plotmm/paint.h>
#include <plotmm/symbol.h>
#include <sigc++/functors/mem_fun.h>

PlotAxisChoice::PlotAxisChoice(int def)
{
	append("p");
	append("T");
	append("v");
	append("u");
	append("h");
	append("s");

	set_active(def);
}

enum PlotAxisQuantity PlotAxisChoice::get_quantity()
{
	return static_cast<enum PlotAxisQuantity>(get_active_row_number());
}

PlotAxesControl::PlotAxesControl()
	: x(1), y(0), sep("-")
{
	x.signal_changed().connect(
			sigc::mem_fun(*this, &PlotAxesControl::axis_changed));
	y.signal_changed().connect(
			sigc::mem_fun(*this, &PlotAxesControl::axis_changed));

	set_spacing(5);

	pack_start(y);
	pack_start(sep, Gtk::PACK_SHRINK);
	pack_start(x);
}

void PlotAxesControl::axis_changed()
{
	axes_changed.emit(x.get_quantity(), y.get_quantity());
}

PlotAxesControl::axes_changed_sig PlotAxesControl::signal_axes_changed()
{
	return axes_changed;
}

SaturationCurve::SaturationCurve()
{
	paint()->set_pen_color(Gdk::Color("blue"));
}

void SaturationCurve::replot(PlotAxisProperty x_prop, PlotAxisProperty y_prop)
{
	const int len = 623 - 273 + 1;
	double x[len*2], y[len*2];

	int i;

	for (i = 0; i < len; ++i)
	{
		double T = 273.15 + i;
		h2o::H2O water = h2o::H2O::Tx(T, 0);
		h2o::H2O steam = h2o::H2O::Tx(T, 1);

		x[i] = (water.*x_prop)();
		y[i] = (water.*y_prop)();

		x[2*len - i - 1] = (steam.*x_prop)();
		y[2*len - i - 1] = (steam.*y_prop)();
	}

	set_data(x, y, 2*len);
}

#include <iostream>

DataCurve::DataCurve()
{
	symbol()->paint()->set_pen_color(Gdk::Color("red"));
	symbol()->set_style(PlotMM::SYMBOL_CROSS);
	symbol()->set_size(5);
}

void DataCurve::replot(PlotAxisProperty x_prop, PlotAxisProperty y_prop,
			std::vector<h2o::H2O>& data)
{
	const int len = data.size();

	double x[len], y[len];

	int i;

	for (i = 0; i < len; ++i)
	{
		x[i] = (data[i].*x_prop)();
		y[i] = (data[i].*y_prop)();
	}

	set_data(x, y, len);
}

Plot::Plot()
	: x_prop(&h2o::H2O::T), y_prop(&h2o::H2O::p),
	saturation_curve(new SaturationCurve()),
	data_curve(new DataCurve())
{
	set_size_request(300, 200);

	scale(PlotMM::AXIS_LEFT)->set_autoscale(true);
	scale(PlotMM::AXIS_BOTTOM)->set_autoscale(true);
	scale(PlotMM::AXIS_RIGHT)->set_enabled(false);

	add_curve(saturation_curve);
	add_curve(data_curve);

	saturation_curve->replot(x_prop, y_prop);
	replot();
}

static PlotAxisProperty quant_to_prop(enum PlotAxisQuantity q)
{
	switch (q)
	{
		case pa_p:
			return &h2o::H2O::p;
		case pa_T:
			return &h2o::H2O::T;
		case pa_v:
			return &h2o::H2O::v;
		case pa_u:
			return &h2o::H2O::u;
		case pa_h:
			return &h2o::H2O::h;
		case pa_s:
			return &h2o::H2O::s;
		default:
			assert(!"Invalid plot axis");
	}
}

void Plot::update_axes(enum PlotAxisQuantity x, enum PlotAxisQuantity y)
{
	x_prop = quant_to_prop(x);
	y_prop = quant_to_prop(y);

	saturation_curve->replot(x_prop, y_prop);
	data_curve->replot(x_prop, y_prop, current_data);
	replot();
}

void Plot::plot_data(h2o::H2O data[], int len)
{
	int i;

	current_data.reserve(len);
	current_data.clear();
	for (i = 0; i < len; ++i)
		current_data.push_back(data[i]);

	data_curve->replot(x_prop, y_prop, current_data);
	replot();
}

PlotBox::PlotBox()
{
	axes_control.signal_axes_changed().connect(
			sigc::mem_fun(plot, &Plot::update_axes));

	pack_start(axes_control, Gtk::PACK_SHRINK, 2);
	pack_start(plot);
}

void PlotBox::update_data_plot(h2o::H2O* data, int len)
{
	plot.plot_data(data, len);
}
