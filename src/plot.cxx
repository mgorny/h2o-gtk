/* h2o-gtk+ -- GTK+ UI to libh2o
 * (c) 2012 Michał Górny
 * Released under the terms of the 2-clause BSD license
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#ifdef HAVE_PLOTMM

#include "plot.hxx"

#include <vector>
#include <cassert>

#include <gdkmm/color.h>
#include <gtkmm/stock.h>
#include <plotmm/paint.h>
#include <plotmm/symbol.h>
#include <sigc++/functors/mem_fun.h>

PlotAxisChoice::PlotAxisChoice(int def)
{
	append_text("p");
	append_text("T");
	append_text("v");
	append_text("u");
	append_text("h");
	append_text("s");
	append_text("\317\201");

	set_active(def);
}

enum PlotAxisQuantity PlotAxisChoice::get_quantity()
{
	return static_cast<enum PlotAxisQuantity>(get_active_row_number());
}

PlotAxesControl::PlotAxesControl()
	: x(5), y(1), sep("-")
{
	x.signal_changed().connect(
			sigc::mem_fun(*this, &PlotAxesControl::axis_changed));
	y.signal_changed().connect(
			sigc::mem_fun(*this, &PlotAxesControl::axis_changed));

	x.set_tooltip_text("x axis quantity");
	y.set_tooltip_text("y axis quantity");

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
	const double Tmin = 273.15;
	const double Tmax = 647.096;

	const int len = 647 - 273;
	double x[len*2+1], y[len*2+1];

	int i;

	for (i = 0; i < len; ++i)
	{
		double T = Tmin + i;
		h2o::H2O water = h2o::H2O::Tx(T, 0);
		h2o::H2O steam = h2o::H2O::Tx(T, 1);

		x[i] = (water.*x_prop)();
		y[i] = (water.*y_prop)();

		x[2*len - i - 1] = (steam.*x_prop)();
		y[2*len - i - 1] = (steam.*y_prop)();
	}

	h2o::H2O crit = h2o::H2O::Tx(Tmax, 0);
	x[i] = (crit.*x_prop)();
	y[i] = (crit.*y_prop)();

	set_data(x, y, 2*len);
}

DataCurve::DataCurve(const char* color)
{
	symbol()->paint()->set_pen_color(Gdk::Color(color));
	symbol()->set_style(PlotMM::SYMBOL_CROSS);
	symbol()->set_size(5);
}

std::vector<h2o::H2O>& DataCurve::get_data()
{
	return data;
}

void DataCurve::replace_data(std::vector<h2o::H2O>& new_data)
{
	data = new_data;
}

void DataCurve::replace_data(h2o::H2O* new_data, int len)
{
	int i;

	data.reserve(len);
	data.clear();
	for (i = 0; i < len; ++i)
		data.push_back(new_data[i]);
}

void DataCurve::replot(PlotAxisProperty x_prop, PlotAxisProperty y_prop)
{
	const int len = data.size();

	std::vector<double> x, y;

	int i;

	for (i = 0; i < len; ++i)
	{
		double xval, yval;

		try
		{
			xval = (data[i].*x_prop)();
			yval = (data[i].*y_prop)();
		}
		catch (std::runtime_error)
		{
			continue;
		};

		x.push_back(xval);
		y.push_back(yval);
	}

	if (x.size() > 0)
	{
		set_data(x, y);
		set_enabled(true);
	}
	else
		set_enabled(false);
}

Plot::Plot()
	: x_prop(&h2o::H2O::s), y_prop(&h2o::H2O::T),
	saturation_curve(new SaturationCurve()),
	data_curve(new DataCurve("red"))
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
	PlotAxisProperty ret;

	switch (q)
	{
		case pa_p:
			ret = &h2o::H2O::p;
			break;
		case pa_T:
			ret = &h2o::H2O::T;
			break;
		case pa_v:
			ret = &h2o::H2O::v;
			break;
		case pa_u:
			ret = &h2o::H2O::u;
			break;
		case pa_h:
			ret = &h2o::H2O::h;
			break;
		case pa_s:
			ret = &h2o::H2O::s;
			break;
		case pa_rho:
			ret = &h2o::H2O::rho;
			break;
		default:
			assert(!"Invalid plot axis");
	}

	return ret;
}

void Plot::update_axes(enum PlotAxisQuantity x, enum PlotAxisQuantity y)
{
	x_prop = quant_to_prop(x);
	y_prop = quant_to_prop(y);

	saturation_curve->replot(x_prop, y_prop);
	data_curve->replot(x_prop, y_prop);

	DataCurveVector::iterator i;
	for (i = user_plot.begin(); i != user_plot.end(); ++i)
		(*i)->replot(x_prop, y_prop);

	replot();
}

void Plot::plot_data(h2o::H2O data[], int len)
{
	data_curve->replace_data(data, len);
	data_curve->replot(x_prop, y_prop);
	replot();
}

void Plot::append_plot()
{
	Glib::RefPtr<DataCurve> new_curve(
			new DataCurve("green"));

	new_curve->replace_data(data_curve->get_data());
	new_curve->replot(x_prop, y_prop);
	add_curve(new_curve);
	user_plot.push_back(new_curve);
	replot();
}

void Plot::clear_plot()
{
	// XXX: can we remove curves from plot somehow?
	// seems like we have to just keep them disabled...

	DataCurveVector::iterator i;
	for (i = user_plot.begin(); i != user_plot.end(); ++i)
	{
		(*i)->set_data(NULL, NULL, 0);
		(*i)->set_enabled(false);
	}

	user_plot.clear();
	replot();
}

PlotBottomControlBox::PlotBottomControlBox()
	: Gtk::Table(1, 2),
	add_to_plot(Gtk::Stock::ADD),
	clear_plot(Gtk::Stock::CLEAR)
{
	add_to_plot.signal_clicked().connect(add_to_plot_sig);
	clear_plot.signal_clicked().connect(clear_plot_sig);

	add_to_plot.set_tooltip_text("Add currently rendered function to the plot");
	clear_plot.set_tooltip_text("Remove all plot data");

	attach(add_to_plot, 0, 1, 0, 1);
	attach(clear_plot, 1, 2, 0, 1);
}

PlotBottomControlBox::button_sig PlotBottomControlBox::signal_add_to_plot()
{
	return add_to_plot_sig;
}

PlotBottomControlBox::button_sig PlotBottomControlBox::signal_clear_plot()
{
	return clear_plot_sig;
}

PlotBox::PlotBox()
{
	axes_control.signal_axes_changed().connect(
			sigc::mem_fun(plot, &Plot::update_axes));
	bottom_control.signal_add_to_plot().connect(
			sigc::mem_fun(plot, &Plot::append_plot));
	bottom_control.signal_clear_plot().connect(
			sigc::mem_fun(plot, &Plot::clear_plot));

	pack_start(axes_control, Gtk::PACK_SHRINK, 2);
	pack_start(plot);
	pack_start(bottom_control, Gtk::PACK_SHRINK, 2);
}

void PlotBox::update_data_plot(h2o::H2O* data, int len)
{
	plot.plot_data(data, len);
}

#endif /*HAVE_PLOTMM*/
