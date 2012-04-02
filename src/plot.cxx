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
#include <sigc++/functors/mem_fun.h>

PlotAxisChoice::PlotAxisChoice(int def)
{
	append("p");
	append("T");

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
	double x[len], y[len];

	int i;

	for (i = 0; i < len; ++i)
	{
		double T = 273.15 + i;
		h2o::H2O water = h2o::H2O::Tx(T, 0);

		x[i] = (water.*x_prop)();
		y[i] = (water.*y_prop)();
	}

	set_data(x, y, len);
}

Plot::Plot()
	: x_prop(&h2o::H2O::T), y_prop(&h2o::H2O::p),
	saturation_curve(new SaturationCurve())
{
	set_size_request(300, 200);

	scale(PlotMM::AXIS_LEFT)->set_autoscale(true);
	scale(PlotMM::AXIS_BOTTOM)->set_autoscale(true);
	scale(PlotMM::AXIS_RIGHT)->set_enabled(false);

	add_curve(saturation_curve);

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
		default:
			assert(!"Invalid plot axis");
	}
}

void Plot::update_axes(enum PlotAxisQuantity x, enum PlotAxisQuantity y)
{
	x_prop = quant_to_prop(x);
	y_prop = quant_to_prop(y);

	saturation_curve->replot(x_prop, y_prop);
	replot();
}

PlotBox::PlotBox()
{
	axes_control.signal_axes_changed().connect(
			sigc::mem_fun(plot, &Plot::update_axes));

	pack_start(axes_control, Gtk::PACK_SHRINK, 2);
	pack_start(plot);
}
