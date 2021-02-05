#pragma once
#include <rack.hpp>

using namespace rack;

extern Plugin *pluginInstance;

extern Model *modelComparator;
extern Model *modelMuxDemux;
extern Model *modelClock;
extern Model *modelSequencer;
extern Model *modelDivider;
extern Model *modelRandom;
extern Model *modelADSR;
extern Model *modelEnvFollower;

struct Label : TransparentWidget {
  std::shared_ptr<Font> font;
  NVGcolor color;
  char text[128];
  const int size = 14;

  Label(Vec pos) {
    box.pos = pos;
    box.size.y = size;
    setColor(0x00, 0x00, 0x00, 0xFF);
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/FiraCode-Regular.ttf"));
    setText(" ");
  }

  Label(Vec pos, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    box.pos = pos;
    box.size.y = size;
    setColor(r, g, b, a);
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/FiraCode-Regular.ttf"));
    setText(" ");
  }

  void setText(const char *txt) {
    strncpy(text, txt, sizeof(text));
    box.size.x = strlen(text) * 8;
  }

  void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
  }

  void drawBackground(const DrawArgs &args) {
    const int whalf = box.size.x / 2;
    Vec c = Vec(whalf, box.size.y);
    nvgFillColor(args.vg, nvgRGBA(0xf0, 0xf0, 0xf0, 0xff));
    {
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, c.x - whalf, c.y + 2);
      nvgMoveTo(args.vg, c.x + whalf, c.y + 2);
      nvgMoveTo(args.vg, c.x + whalf, c.y + size + 2);
      nvgMoveTo(args.vg, c.x - whalf, c.y + size + 2);
      nvgMoveTo(args.vg, c.x - whalf, c.y + 2);
      nvgClosePath(args.vg);
    }
    nvgFill(args.vg);
  }

  void drawTxt(const DrawArgs &args, const char *txt) {
    Vec c = Vec(box.size.x / 2, box.size.y);
    nvgFontSize(args.vg, size);
    nvgFontFaceId(args.vg, font->handle);
    // nvgTextLetterSpacing(args.vg, -2);
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
    nvgFillColor(args.vg, nvgRGBA(color.r, color.g, color.b, color.a));
    nvgText(args.vg, c.x, c.y + size, txt, NULL);
  }

  void draw(const DrawArgs &args) override {
    TransparentWidget::draw(args);
    drawBackground(args);
    drawTxt(args, text);
  }
};

struct Title : TransparentWidget {
  std::shared_ptr<Font> font;
  NVGcolor color;
  char text[128];
  int size = 20;
  float parentWidth = 0;

  Title(float pW) {
    parentWidth = pW;
    box.pos = Vec(0, 3);
    box.size.y = size;
    setColor(0x00, 0x00, 0x00, 0xff);
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/FiraCode-Regular.ttf"));
    setText(" ");
  }

  void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
  }

  void setText(const char *txt) {
    strncpy(text, txt, sizeof(text));
    box.size.x = strlen(text) * 10;
  }

  void draw(const DrawArgs &args) override {
    TransparentWidget::draw(args);
    drawTxt(args, text);
  }

  void drawTxt(const DrawArgs &args, const char *txt) {
    float bounds[4];
    Vec c = Vec(box.pos.x, box.pos.y);

    nvgFontSize(args.vg, size);
    nvgFontFaceId(args.vg, font->handle);
    // nvgTextLetterSpacing(args.vg, -2);
    nvgTextAlign(args.vg, NVG_ALIGN_LEFT);
    nvgTextBounds(args.vg, c.x, c.y, txt, NULL, bounds);

    float xmax = bounds[2];
    if (xmax > parentWidth) {
      float ratio = parentWidth / xmax;
      size = (int)floor(ratio * size);
    } else {
      c.x += (parentWidth - xmax) / 2;
    }

    nvgFillColor(args.vg, nvgRGBA(color.r, color.g, color.b, color.a));
    nvgText(args.vg, c.x, c.y + size, txt, NULL);
  }
};

struct RoundBlackKnobWithLabel : RoundBlackKnob {
  std::shared_ptr<Font> font;
  NVGcolor color;

  RoundBlackKnobWithLabel() {
    setColor(0x00, 0x00, 0x00, 0xff);
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/FiraCode-Regular.ttf"));
  }

  RoundBlackKnobWithLabel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    setColor(r, g, b, a);
    font = APP->window->loadFont(asset::plugin(pluginInstance, "res/FiraCode-Regular.ttf"));
  }

  void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
  }

  void draw(const DrawArgs &args) override {
    char buf[128];
    ParamWidget::draw(args);
    snprintf(buf, sizeof(buf), "%.3G", dirtyValue);
    drawLabel(args, buf);
  }

  void drawLabel(const DrawArgs &args, const char *txt) {
    Vec c = Vec(box.size.x / 2, box.size.y);
    const int size = 14;
    const int whalf = 15;

    nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0xf0));
    {
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, c.x - whalf, c.y + 2);
      nvgLineTo(args.vg, c.x + whalf, c.y + 2);
      nvgQuadTo(args.vg, c.x + whalf + 5, c.y + 9, c.x + whalf, c.y + size + 2);
      nvgLineTo(args.vg, c.x - whalf, c.y + size + 2);
      nvgQuadTo(args.vg, c.x - whalf - 5, c.y + 8, c.x - whalf, c.y + 2);
      nvgClosePath(args.vg);
    }

    nvgFill(args.vg);
    nvgFillColor(args.vg, nvgRGBA(0x00, 0x00, 0x00, 0x0f));
    {
      nvgBeginPath(args.vg);
      nvgMoveTo(args.vg, c.x - whalf, c.y + 2);
      nvgLineTo(args.vg, c.x + whalf, c.y + 2);
      nvgQuadTo(args.vg, c.x + whalf + 5, c.y + 2 + 7, c.x + whalf, c.y + size + 2);
      nvgLineTo(args.vg, c.x - whalf, c.y + size + 2);
      nvgQuadTo(args.vg, c.x - whalf - 5, c.y + 2 + 7, c.x - whalf, c.y + 2);
      nvgClosePath(args.vg);
    }
    nvgStrokeWidth(args.vg, 0.5);
    nvgStroke(args.vg);

    nvgFontSize(args.vg, size);
    nvgFontFaceId(args.vg, font->handle);
    nvgTextLetterSpacing(args.vg, -2);
    nvgTextAlign(args.vg, NVG_ALIGN_CENTER);
    nvgFillColor(args.vg, nvgRGBA(color.r, color.g, color.b, color.a));

    nvgText(args.vg, c.x, c.y + size, txt, NULL);
  }
};
