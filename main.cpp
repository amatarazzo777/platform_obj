#include "uxdevice.hpp"

using namespace std;
using namespace uxdevice;

void drawText(SurfaceArea &vis, bool bfast);
void drawshapes(SurfaceArea &vis);
void drawimages(SurfaceArea &vis);
void drawText(SurfaceArea &vis, bool bfast, string stxt);
void drawlines(SurfaceArea &vis);

void eventDispatch(const event &evt);
void handleError(const std::string errText) {
  fprintf(stderr, "%s", errText.data());
}

// inline PNG data using the RFC2397 base 64 encoding scheme.
// This may be useful in some cases, yet raw byte data will be smaller in memory
// description but larger in the source description perhaps unless the c++ RAW
// literanl input format is used for literal string data.
const char *stripes =
    "data:image/"
    "png;base64,iVBORw0KGgoAAAANSUhEUgAAACsAAAARCAYAAABEvFULAAAABmJLR0QA/wD/"
    "AP+"
    "gvaeTAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH5AQZFBsOzDGg0AAABQZJREFUSMe"
    "llttvVFUUxn9r98x0oLRAW8pNLKUBrG1pC3ItEIqAQNSoGCEQQQNG/"
    "gvfjD74QIwmPpCIGgWCJiogQaQgUAWDUC4lpUCBWq51GEovM9Nz9vLhdJiZthQMK5mH2Ze1v"
    "rXWt759ZMtW52Z3F6PFgOn9Jcx1wXOT/"
    "wPB9P1UcxxYstJQkLsIS4RvvvwbgMwQLHvZ0NqiXG9W7kfQ7i7/"
    "fCAIBaNFSiuEvLyxBGQqrl5j357L3Lnln0ngmFIiyOdfO3bWPEP4X+"
    "XiBSUWTQKYPlt4vmQmRvJAY5w5X8v5eiURrK9ZC8EgxOPJpFzXP5tfALl5wsg8wfOUC2eV+"
    "5Gkn564f/"
    "aFuUJl1WQaG5u41qxEwhCL+"
    "r7l488cm6iM40A0mg4kFoW31huyh1Yi5CCEuH5jP8ePWh60Dww6FfyUEmHaDCEzMJYMGQ0Yh"
    "KEIQ4npr/"
    "zyo0esN6brQmeHf7dihjC9qhohBDh4XEVauoNeppkDBAGLp618v+OypFY4USHwg8+"
    "cZwhKNY1NRzhxTHlSS/hQC5MmCwtrSjDkI6RnfPpsLQ31SmcHbHg/"
    "m4BU+Em2RvO8wwfauXtbJdVhKhcTrXzU3tNYIAjViwzjxz2HIT9tr7HpCHWHFWNg3bvjkQ+"
    "3+DSYWCxUzBDNHTlBRbJR7aSh4ar8dUxFTP92DwbUWn8wrU2uGQMD+UmtenaOX/"
    "HSCiFkFgEGxeWXvYdouwNyIzrWO3miTe7dU7KzhbnVUzWZoQNYLGH2/"
    "HROIuEnr1ZRsTBndo0qcZQ44GL1AUoHFxpuyfEjfsUCwf6JpiaZKAyAfPSpYzMcP/"
    "NEpRYsNhQWZXL2dJQZVTUKcPRYrVxpenJ+"
    "JgIuf9UwJn9x70WLkkDiz0dLSzMH9loZTBYTJp984TzMIzcfhgwRrjUrnutrpOtCeZVwqVHp"
    "if9/Tib43qupjBqDFhaJZA0pxJFiFFfBlXB7HQf2Wjo7BqZKIAiyc1/"
    "A5hX4OldeOYyAVGmk46hkZQ1n+7Z7TzU8rtv/"
    "oUl9bHJGwJtrpmAYB1iEIM3XD1K73w44I3I3tsxTXJQOlDiq3VhusX9Pp4Tb+"
    "svOYLra19ZsGKJnT0el/qQ+8l5PHNa/"
    "NwpHSlLT4VDtHzRfTr8nu48G7LlTPqXEPBpQaYXotMpxcrzuBq0t6S/doywWhU2b56mnt/"
    "lq62UZTAne3jgeR4pTVg2eXuPwwau03fXjmcbzSiDocyLxig1k9SdVhJBWV1fr6jVLNL/"
    "g8WAzQ+BqE44UMiK3P8DqRQZr/ZjfbWtF6UgdUTJkAotfXMCq1cX0xCHjpVfMB0/"
    "SUmPgYuM9KS2bBMSZPPlZyiqU0kqPCUWeRMJK+/3+/"
    "Gxq7JKysiJi7lW5fbOP7gqsfC1Lz5zqERG40HCTsvLCvpFpunSaG/"
    "+AGeg5HKytu3YekrZInQghMqSQgFSRP2K+rli5UN/ZNF3fWGs0lSId7eBxh/"
    "Lycu1LnStNSlDmsmqtUWt9/"
    "loi6XKFw5+9mmwSmjh2vLBuY4b2FeS+1t0FP++y7Nh+QO6GfdBge6d5KFnB+"
    "WzcPF1HjUlKjqfNZJDPMxPTfamFHq1nWGg+r68xCrDz2zPpdNHmh5+"
    "pBmD0OFi6tEaDMpspJfJYSjiOX+XdP/"
    "igw+11JJvkg16xfLFmhvyVSxe7RLHMmmfIGZHs4LAcej9SXOp+t5JQh4O/HektABgZ/"
    "jDuf4YrMMm0cOGnAAAAAElFTkSuQmCC";

// inline SVG, you may build this at run time or parameterize certain aspects
// of the style or and of its data. Not all of the blocks below are required,
// such as meta data. The parsing is fast, however the information is still
// transposed from text. The biggest slowdowns are the effects that use the
// Gaussian blur functions. This uses the RSVG api for its rendering and
// parsing. Files may be drawn in the inkscape application and transposed at
// this layer.
std::string sSVG =
    R"data(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:xlink="http://www.w3.org/1999/xlink"
   id="svg8"
   version="1.1"
   viewBox="0 0 210 297"
   height="297mm"
   width="210mm">
  <defs
     id="defs2">
    <linearGradient
       id="linearGradient8910">
      <stop
         id="stop8906"
         offset="0"
         style="stop-color:#00ff00;stop-opacity:1;" />
      <stop
         id="stop8908"
         offset="1"
         style="stop-color:#00ff00;stop-opacity:0;" />
    </linearGradient>
    <linearGradient
       id="linearGradient825">
      <stop
         id="stop821"
         offset="0"
         style="stop-color:#00ffff;stop-opacity:1;" />
      <stop
         id="stop823"
         offset="1"
         style="stop-color:#00ffff;stop-opacity:0;" />
    </linearGradient>
    <linearGradient
       gradientTransform="translate(-12.095238,-27.214286)"
       gradientUnits="userSpaceOnUse"
       y2="73.316124"
       x2="64.48204"
       y1="172.34589"
       x1="63.112759"
       id="linearGradient827"
       xlink:href="#linearGradient825" />
    <radialGradient
       gradientUnits="userSpaceOnUse"
       gradientTransform="matrix(1,0,0,1.1016153,0,-15.042195)"
       r="19.052965"
       fy="148.03078"
       fx="25.656507"
       cy="148.03078"
       cx="25.656507"
       id="radialGradient8914"
       xlink:href="#linearGradient8910" />
  </defs>
  <metadata
     id="metadata5">
    <rdf:RDF>
      <cc:Work
         rdf:about="">
        <dc:format>image/svg+xml</dc:format>
        <dc:type
           rdf:resource="http://purl.org/dc/dcmitype/StillImage" />
        <dc:title></dc:title>
      </cc:Work>
    </rdf:RDF>
  </metadata>
  <g
     id="layer1">
    <path
       d="M 99.785711,89.869046 65.017486,84.07598 40.534658,109.43303 35.300219,74.576329 3.6186199,59.127536 35.151783,43.377975 40.054307,8.473044 64.777312,33.595983 99.488837,27.472343 83.235332,58.748733 Z"
       id="path815"
       style="opacity:1;fill:url(#linearGradient827);fill-opacity:1;stroke:#0085ec;stroke-width:0.86500001;stroke-linecap:round;stroke-linejoin:bevel;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;" />
    <path
       transform="matrix(3.4653861,0,0,2.8495594,-16.384137,-312.53425)"
       d="m 27.59226,149.96727 c 0.397975,-1.09917 1.677999,-0.006 1.826885,0.66146 0.40347,1.80831 -1.571248,3.05425 -3.149804,2.99231 -2.823666,-0.1108 -4.519539,-3.03841 -4.157734,-5.63815 0.530964,-3.81521 4.520678,-6.01497 8.126491,-5.32316 4.805983,0.92207 7.522436,6.00767 6.488584,10.61484 -1.300996,5.79764 -7.496768,9.03597 -13.103179,7.65401 -6.790114,-1.67375 -10.55299,-8.98702 -8.819433,-15.59153 2.042936,-7.78318 10.477978,-12.07219 18.079866,-9.98486 8.776682,2.40991 13.59285,11.96942 11.150284,20.56822 -2.775398,9.77049 -13.461191,15.11452 -23.056555,12.3157 C 10.213117,165.09626 4.3407206,153.2829 7.496532,142.69122 11.000081,130.93244 23.941951,124.5313 35.529775,128.04466"
       id="path8711"
       style="fill:url(#radialGradient8914);fill-opacity:1;fill-rule:evenodd;stroke:#000000;stroke-width:0.08419723;" />
    <path
       id="path8713"
       d="m 106.74025,52.336607 c 51.2538,42.068154 52.00975,1.624703 52.7657,50.761603 0.75595,49.13691 13.22917,27.21429 1.51191,48.38095 -11.71727,21.16667 -28.34822,41.57738 -20.03274,6.4256 8.31547,-35.15178 -6.04762,-27.97024 7.9375,-40.44345 13.98512,-12.47322 26.83631,8.69345 26.83631,8.69345"
       style="fill:#ff0000;fill-opacity:0;stroke:#000000;stroke-width:0.26458332px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1;" />
    <path
       id="path8821"
       d="m 138.64911,112.25335 c -2.54068,0.10491 -3.03691,-4.34524 -1.8739,-5.25183 -2.63067,-2.99434 3.7364,-4.48073 4.99197,-6.28774 1.39769,-1.608881 -6.31105,1.58184 -4.55268,-1.882209 1.11045,-1.442297 -0.38956,-4.324543 0.83936,-6.16917 1.00714,-1.649731 2.81516,-3.346919 3.36901,-4.621129 2.20741,-2.415567 -3.22176,-3.97794 -0.25915,-6.398655 1.19745,-1.184747 1.82828,-4.859365 3.5587,-3.505948 1.78639,-1.776738 2.836,1.134619 4.61727,0.70695 2.29649,1.14531 3.40698,3.853626 4.79277,6.027843 1.61057,1.8066 3.95398,2.083252 6.00743,2.846152 0.84936,-1.06443 3.42631,-0.214252 1.92488,0.77227 -1.68048,-0.552873 -3.5271,2.141067 -1.04187,2.649209 2.38537,1.946136 5.08802,2.611278 7.89105,2.585799 2.72759,0.514352 5.46843,-0.217895 8.09459,-0.991072 1.1792,-0.993413 2.31298,-1.433339 3.72499,-0.909305 1.52384,-1.092216 4.12865,-0.873475 6.14013,-1.031972 1.23129,-1.789946 3.44005,0.330912 2.09625,2.031677 0.13701,3.02278 -0.60075,5.931311 -2.94384,7.41655 -1.16643,0.88749 -0.45089,1.04475 0.23998,1.21575 -0.1081,1.35324 -3.24806,3.43646 -0.39057,3.05985 -0.66436,0.74825 0.31666,1.68944 0.18053,1.72477 -2.23325,0.47006 -4.18343,2.01719 -6.46928,2.03152 -4.64177,0.25668 -9.24635,1.01947 -13.83033,1.86064 -1.86085,-1.71727 -4.36461,-0.28593 -5.39546,1.26524 -0.37139,-1.40386 -0.77796,-3.06523 -2.45487,-2.07418 -1.95871,0.89052 -4.51407,0.77245 -6.1767,0.91127 -1.77315,2.30965 -4.28944,-0.40836 -6.42742,-0.0104 -2.27014,0.23561 -4.31716,3.70352 -6.65284,2.02806 z m 1.7307,-4.24352 c -1.09146,-1.41181 0.23917,1.60861 0,0 z m -1.81029,-0.25856 c -0.79132,-1.8222 -0.65971,1.70898 0,0 z m 3.40178,-5.78404 c -0.49055,-1.84081 -1.34096,0.9701 0,0 z m 46.1131,-4.543848 c -1.34206,-1.886642 -1.25139,1.834602 0,0 z m 0.56655,-4.302531 c 0.62603,-2.222978 -3.53194,-0.974369 -1.09785,0.226802 0.32759,0.334043 0.97603,0.355752 1.09785,-0.226802 z m -2.45643,-1.983274 c -1.55204,-1.07211 -1.11827,1.669547 0,0 z"
       style="opacity:1;fill:#ff0000;fill-opacity:1;stroke:#4685ec;stroke-width:0.86499995;stroke-linecap:round;stroke-linejoin:bevel;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;" />
    <path
       d="M 117.33132,95.746045 84.328999,35.758881 75.694158,72.873818 122.54707,22.949697 84.58035,26.206622 146.53936,55.338983 131.70948,20.236935 123.14934,88.165846 151.95068,63.214663 84.701225,76.064677 Z"
       id="path8856"
       style="opacity:1;fill:#0000ff;fill-opacity:1;stroke:#4685ec;stroke-width:0.86500001;stroke-linecap:round;stroke-linejoin:bevel;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1;" />
    <rect
       ry="14.174099"
       rx="17.41297"
       y="82.649841"
       x="7.2162776"
       height="33.408691"
       width="84.724442"
       id="rect8880"
       style="opacity:1;fill:#ffff00;fill-opacity:1;stroke:#4685ec;stroke-width:0.86500001;stroke-linecap:round;stroke-linejoin:bevel;stroke-miterlimit:4;stroke-dasharray:none;stroke-opacity:1" />
  </g>
</svg>
)data";

/****************************************************************************************************
***************************************************************************************************/

std::random_device rd;
std::mt19937 gen(rd());
// events such as mouse and keyboard input are distributed here.
// The area named during the event is tied to the mouse and keyboard
// focus input.
double sx = 1, sy = 1;
double mx = 0, my = 0;
double ox = 0, oy = 0;

#define FAST_TEXT true
#define DRAW_SLEEP 1000
#define NUM_SEGMENTS 10

#if defined(__linux__)
int main(int argc, char **argv) {
  // handle command line here...
#elif defined(_WIN64)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /* hPrevInstance */,
                   LPSTR lpCmdLine, int /* nCmdShow */) {
  // command line
#endif

  std::uniform_int_distribution<> motime(500, 5000);

  std::uniform_real_distribution<> color(0, 1.0);
  std::uniform_real_distribution<> opac(.9, 1.0);

  std::uniform_real_distribution<> coord(25.0, 100.0);
  std::uniform_real_distribution<> ang(-10, 10);
#define _C color(gen)
#define _A opac(gen)

  SurfaceArea vis = SurfaceArea(
      {500, 500}, "Information Title",
      Paint(coord(gen), coord(gen), coord(gen), coord(gen),
            {{_C, _C, _C, _C, 1}, {_C, _C, _C, _C, 1}, {_C, _C, _C, _C, 1}}));

  vis.listen(eventType::keypress, [&vis](auto &evt) {
    string s = " ";
    s[0] = evt.key;
    drawText(vis, true, s);
  });

  vis.listen(eventType::mousemove, [](auto &evt) {
    if (evt.y < my) {
      oy = oy - .1;
    } else {
      oy = oy + .1;
    }
    if (oy < 0)
      oy = 0;
    // vis.deviceOffset(ox,oy);
    mx = evt.x;
    my = evt.y;
  });

  drawText(vis, FAST_TEXT);
  drawlines(vis);

  vis.notify_complete();
  while (vis.processing()) {
    vis.clear();
    drawlines(vis);
    drawText(vis, FAST_TEXT);

    vis.notify_complete();
    std::this_thread::sleep_for(std::chrono::milliseconds(DRAW_SLEEP));
  }

  return 0;
}

void drawText(SurfaceArea &vis, bool bfast, string stxt) {

  std::uniform_real_distribution<> scrn(0, 400.0);
  std::uniform_real_distribution<> dcir(25.0, 100.0);
  std::uniform_real_distribution<> color(.5, 1.0);
  std::uniform_real_distribution<> opac(.7, 1);
  std::uniform_real_distribution<> lw(0, 10.0);
  std::uniform_real_distribution<> coord(425.0, 600.0);
  std::uniform_int_distribution<> shape(1, 4);

#define _C color(gen)
#define _A opac(gen)

  // set the font name according to pango api. see pango font description.
  std::uniform_int_distribution<> fill(1, 2);

  if (bfast) {
    std::time_t t = std::time(nullptr);
    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t));

    vis << coordinates{0, 0} << source("blue") << mbstr << '\n';

  } else {

    vis << text_fill{coord(gen),
                     coord(gen),
                     coord(gen),
                     coord(gen),
                     {{_C, _C, _C, _C, _A},
                      {_C, _C, _C, _C, _A},
                      {_C, _C, _C, _C, _A}}}
        << text_outline{coord(gen),
                        coord(gen),
                        coord(gen),
                        coord(gen),
                        {{_C, _C, _C, _C, _A},
                         {_C, _C, _C, _C, _A},
                         {_C, _C, _C, _C, _A}}}
        << text_shadow{"green"} << line_width{lw(gen)} << alignment_t::left
        << coordinates{10, 10, 300, 300} << stxt << index_by{"text"};

  }
}

void drawText(SurfaceArea &vis, bool bfast) {

  std::uniform_real_distribution<> scrn(0, 400.0);
  std::uniform_real_distribution<> dcir(25.0, 100.0);
  std::uniform_real_distribution<> color(.5, 1.0);
  std::uniform_real_distribution<> opac(.7, 1);
  std::uniform_real_distribution<> lw(0, 10.0);
  std::uniform_real_distribution<> coord(425.0, 600.0);
  std::uniform_int_distribution<> shape(1, 4);

  // set the font name according to pango spi. see pango font description.
  vis << text_font("50px") << text_alignment::left;
  std::uniform_int_distribution<> fill(1, 3);
  vis << coordinates({coord(gen), coord(gen), coord(gen), coord(gen)});

  if (bfast) {
    vis << text_shadow_none{}
        << text_fill_none{}
        << text_outline_none{}
        << source{coord(gen),
                    coord(gen),
                    coord(gen),
                    coord(gen),
                    {{_C, _C, _C, _C, _A},
                     {_C, _C, _C, _C, _A},
                     {_C, _C, _C, _C, _A}}};

  } else {

    vis << text_shadow("black")


   << text_fill(
        coord(gen), coord(gen), coord(gen), coord(gen),
        {{_C, _C, _C, _C, _A}, {_C, _C, _C, _C, _A}, {_C, _C, _C, _C, _A}})

    << text_outline(
        coord(gen), coord(gen), coord(gen), coord(gen),
        {{_C, _C, _C, _C, _A}, {_C, _C, _C, _C, _A}, {_C, _C, _C, _C, _A}});


  }

  std::uniform_int_distribution<> info(1, 5);
  switch (info(gen)) {
  case 1:
    vis << "Silver colored crafts from another galaxy seem "
           "curiously welcomed as the memorizing audio waves "
           "produced a canny type of music. A simple ten note. ";
    break;
  case 2:
    vis << "The color of text can be a choice. Yet the appearance is a common "
           "desire.";
    break;
  case 3:
    vis << "Planets orbit the mass, but this is inconsequential of "
           "the heat provided. As children, we find a balance. ";
    break;
  case 4:
    vis << "The sun sets casting its refraction upon the mountain side. ";
    break;
  case 5:
    vis <<
        "The sun sets casting its refraction upon the mountain side. "
        "The glistening oil coats upon the ravens are a remark of healthiness. "
        "One that is pronounced during the day and in the moonlight. "
        "At home, a cave dweller sees this all at once. These are indeed fine "
        "things. "
        "The warmth of the sun decays as thousands of brilliant stars dictate "
        "the continual persistence of the system.  A remarkable sight. A "
        "heavenly "
        "home.";
    break;
  }


}

void drawlines(SurfaceArea &vis) {
#if 0
  std::uniform_real_distribution<> scrn(0, 1000);
  std::uniform_real_distribution<> dcir(5.0, 20.0);
  std::uniform_real_distribution<> dimen(25.0, 300.0);
  std::uniform_real_distribution<> color(0, 1.0);
  std::uniform_real_distribution<> opac(.5, 1);
  std::uniform_real_distribution<> lw(7, 30.0);
  std::uniform_real_distribution<> coord(55.0, 100.0);
  std::uniform_int_distribution<> shape(1, 1);

  vis.move_to(scrn(gen), scrn(gen));
  auto &myshape = vis.group("testgroup");

  for (int c = 0; c < NUM_SEGMENTS; c++) {

    switch (shape(gen)) {
    case 1:
      myshape.line(scrn(gen), scrn(gen));
      break;
    case 2:
      myshape.arc(scrn(gen), scrn(gen), dimen(gen), dimen(gen), dimen(gen));
      break;
    case 3:
      myshape.curve(scrn(gen), scrn(gen), scrn(gen), scrn(gen), scrn(gen),
                    scrn(gen));
      break;
    }
  }
  myshape.close_path();

  myshape.line_width(lw(gen));
  auto ps =
      Paint(coord(gen), coord(gen), coord(gen), coord(gen),
            {{_C, _C, _C, _C, 1}, {_C, _C, _C, _C, 1}, {_C, _C, _C, _C, 1}});

  auto pf =
      Paint(coord(gen), coord(gen), coord(gen), coord(gen),
            {{_C, _C, _C, _C, 1}, {_C, _C, _C, _C, 1}, {_C, _C, _C, _C, 1}});

  myshape.stroke_preserve(ps).fill(pf);

  auto &myshape1 = vis.group("testgroup2");
  myshape1.move_to(10, 10).relative().hline(10).vline(10).hline(-10).vline(-10);
  myshape1.stroke_preserve(ps).fill(pf);
  #endif
}
