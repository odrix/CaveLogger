var margin = { top: 20, right: 20, bottom: 30, left: 50 },
    width = 1000 - margin.left - margin.right,
    height = 500 - margin.top - margin.bottom;

var parseDate = d3.time.format("%Y-%m-%d %H:%M:%S").parse;

var x = d3.time.scale().range([0, width]);
var y = d3.scale.linear().range([height, 0]);

var xAxis = d3.svg.axis().scale(x).orient("bottom");
var yAxis = d3.svg.axis().scale(y).orient("left");

var lineTemperature = d3.svg.line()
    .x(function (d) { return x(d.d); })
    .y(function (d) { return y(d.t); });

var lineHygrometrie = d3.svg.line()
    .x(function (d) { return x(d.d); })
    .y(function (d) { return y(d.h); });


var area = d3.svg.area()
    .x(function (d) { return x(d.d); })
    .y0(height)
    .y1(function (d) { return y(d.t); });

var svg = d3.select("body").append("svg")
    .attr("width", width )
    .attr("height", height)
    .attr('class', 'mainGraph')
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");



d3.json("/Data/CAVEDATA.ENV", function (error, json) {
    if (error) return console.warn(error);
	$(json).each(function (i,item) {
		item.d = parseDate(item.d);
	});
    trace(json);
});

function trace(data) {

    x.domain(d3.extent(data, function (d) { return d.d; }));
    y.domain([d3.min(data, function (d) {return d.t;}) - 5, d3.max(data, function (d) {return d.h;})  - -5]); // l'operateur moins se fais que sur des number alors que le plus s'applique aussi sur des string

    svg.append("g")
        .attr("class", "x axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);

    svg.append("g")
        .attr("class", "y axis")
        .call(yAxis)
      .append("text")
        .attr("transform", "rotate(-90)")
        .attr("y", 6)
        .attr("dy", ".71em")
        .style("text-anchor", "end")
        .text("Temperature (°C)");

    svg.append("path")
        .datum(data)
        .attr("class", "temperature")
        .attr("d", lineTemperature);

    svg.append("path")
        .datum(data)
        .attr("class", "hygrometrie")
        .attr("d", lineHygrometrie);
}