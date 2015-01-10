/*
* Aurora: https://github.com/pixelmatix/aurora
* Copyright (c) 2014 Jason Coon
*
* Portions of this code are adapted from SmartMatrixSwirl by Mark Kriegsman: https://gist.github.com/kriegsman/5adca44e14ad025e6d3b
* https://www.youtube.com/watch?v=bsGBT-50cts
* Copyright (c) 2014 Mark Kriegsman
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef PatternSwirl_H

class PatternSwirl : public Drawable {
private:
    const uint8_t borderWidth = 2;

public:
    void start() {
    }

    unsigned int drawFrame() {
        // Apply some blurring to whatever's already on the matrix
        // Note that we never actually clear the matrix, we just constantly
        // blur it repeatedly.  Since the blurring is 'lossy', there's
        // an automatic trend toward black -- by design.
        uint8_t blurAmount = beatsin8(2, 100, 255);

        // Use two out-of-sync sine waves
        uint8_t  i = beatsin8(27, borderWidth, MATRIX_HEIGHT - borderWidth);
        uint8_t  j = beatsin8(41, borderWidth, MATRIX_WIDTH - borderWidth);
        // Also calculate some reflections
        uint8_t ni = (MATRIX_WIDTH - 1) - i;
        uint8_t nj = (MATRIX_WIDTH - 1) - j;

        // The color of each point shifts over time, each at a different speed.
        uint16_t ms = millis();
        effects.leds[effects.XY(i, j)] += CHSV(ms / 11, 200, 255);
        effects.leds[effects.XY(j, i)] += CHSV(ms / 13, 200, 255);
        effects.leds[effects.XY(ni, nj)] += CHSV(ms / 17, 200, 255);
        effects.leds[effects.XY(nj, ni)] += CHSV(ms / 29, 200, 255);
        effects.leds[effects.XY(i, nj)] += CHSV(ms / 37, 200, 255);
        effects.leds[effects.XY(ni, j)] += CHSV(ms / 41, 200, 255);
        
        blur2d(effects.leds, MATRIX_WIDTH, MATRIX_HEIGHT, blurAmount);

        return 0;
    }
};

#endif
