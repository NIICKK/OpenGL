#version 410
out vec4 colorOut;
uniform double screen_ratio;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform double exponent;

uniform sampler2D tex;

uniform double zoom;
uniform int itr;

vec4 map_to_color(float t, vec2 cord) {
    float r = 9.0 * (1.0 - t) * t * t * t;
    float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
    float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;
    if (cord.x<0.4 && cord.y < 0.3){
        return mix(texture(tex, cord*2), vec4(r, g, b, 1.0),0.8);
    }
    return vec4(r, g, b, 1.0);
}

double calc_real(double zx, double zy, double exp){
    double temp_x, temp_y;
    temp_x = zx; temp_y = zy;
    int i;
    for(i = 0; i < exp -1; i++){
        double temp_xx = temp_x;
   	    temp_x = temp_x * zx - temp_y* zy;
	    temp_y = temp_xx * zy + temp_y* zx;
    }
    return temp_x;
}

double Calc_img(double zx, double zy, double exp){
    double temp_x, temp_y;
    temp_x = zx; temp_y = zy;
    int i;
    for(i = 0; i < exp -1; i++){
        double temp_xx = temp_x;
   	    temp_x = temp_x * zx - temp_y* zy;
	    temp_y = temp_xx * zy + temp_y* zx;
    }
	return temp_y;
}

void main()
{
    dvec2 z, c;
    c.x = screen_ratio * (gl_FragCoord.x / screen_size.x - 0.5);
    c.y = (gl_FragCoord.y / screen_size.y - 0.5);

    c.x /= zoom;
    c.y /= zoom;

    c.x += center.x;
    c.y += center.y;

        z.x = 0;
        z.y = 0;

//    z.x = screen_ratio * (gl_FragCoord.x / screen_size.x - 0.5);
//    z.y = (gl_FragCoord.y / screen_size.y - 0.5);
//
//    z.x /= zoom;
//    z.y /= zoom;
//
//    z.x += center.x;
//    z.y += center.y;
//    c.x = -0.5;
//    c.y = -0.5;

    int i;
    for(i = 0; i < itr; i++) {

    double x = calc_real(z.x,z.y,exponent)  +c.x;
	double y = Calc_img(z.x,z.y,exponent)  + c.y;

	if((x * x + y * y) > 2.0) break;
	z.x = x;
	z.y = y;
    }

    double t = double(i) / double(itr);

    colorOut = map_to_color(float(t), vec2 (gl_FragCoord.x / screen_size.x, 1-gl_FragCoord.y / screen_size.y));
}
