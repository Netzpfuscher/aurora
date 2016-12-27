/*
 * Aurora: https://github.com/pixelmatix/aurora
 *
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from Craig Lindley's LightAppliance:
 * https://github.com/CraigLindley/LightAppliance
 * Copyright (c) 2014 Craig A. Lindley
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

#ifndef IrCodes_H
#define IrCodes_H

enum class InputCommand {
  None,
  Update,
  Up,
  Down,
  Left,
  Right,
  Select,
  CycleBrightness,
  PlayMode,
  Palette,
  CycleClockAndMessageFiles,
  ShowClock,
  HideOverlay,
  Power,
  Back,
  BrightnessUp,
  BrightnessDown,
  Menu,
  ShowCurrentMessage,
  ToggleSettingsMenuVisibility,
  ShowPatternName,
  FreezeDisplay,
};

//unsigned int defaultHoldDelay = 500;
//bool isHolding = false;

InputCommand getCommand(String input) {
  if (input == "Up")
    return InputCommand::Up;
  else if (input == "Down")
    return InputCommand::Down;
  else if (input == "Left")
    return InputCommand::Left;
  else if (input == "Right")
    return InputCommand::Right;
  else if (input == "Select")
    return InputCommand::Select;
  else if (input == "CycleBrightness")
    return InputCommand::CycleBrightness;
  else if (input == "PlayMode")
    return InputCommand::PlayMode;
  else if (input == "Palette")
    return InputCommand::Palette;
  else if (input == "CycleClockAndMessageFiles")
    return InputCommand::CycleClockAndMessageFiles;
  else if (input == "Power")
    return InputCommand::Power;
  else if (input == "Back")
    return InputCommand::Back;
  else if (input == "BrightnessUp")
    return InputCommand::BrightnessUp;
  else if (input == "BrightnessDown")
    return InputCommand::BrightnessDown;
  else if (input == "Menu")
    return InputCommand::Menu;
  else if (input == "ShowCurrentMessage")
    return InputCommand::ShowCurrentMessage;
  else if (input == "ShowClock")
    return InputCommand::ShowClock;
  else if (input == "HideOverlay")
    return InputCommand::HideOverlay;

  return InputCommand::None;
}

void createFile(aJsonObject * root) {
  char path[20];
  uint32_t length = 0;

  aJsonObject* property = aJson.getObjectItem(root, "length");
  if (property->type == aJson_Int) {
    length = property->valueint;
    Serial1.print(F("length: "));
    Serial1.println(length);
  }

  property = aJson.getObjectItem(root, "path");
  if (property->type == aJson_String) {
    strcpy(path, property->valuestring);
    Serial1.print(F("path: "));
    Serial1.println(path);
  }

  if (length < 1) return;

  if (SD.exists(path) && !SD.remove(path)) {
    return;
  }

  File file = SD.open(path, FILE_WRITE);
  if (!file) return;

  uint32_t bytesWritten = 0;

  while (bytesWritten < length) {
    if (Serial1.available() > 0) {
      int b = Serial1.read();
      if (b >= 0) {
        file.write((byte) b);
        bytesWritten++;
      }
    }
  }
  file.close();

  reloadAnimations();
  String name = property->valuestring;
  name = name.substring(6);
  setAnimation(name);
}

InputCommand readSerialCommand() {
  if (Serial1.available() < 1)
    return InputCommand::None;

  if (Serial1.peek() != '{')
    return InputCommand::None;

  aJsonStream stream(&Serial1);

  //Serial.print(F("Parsing json..."));
  aJsonObject* root = aJson.parse(&stream);
  if (!root) {
    //Serial.println(F(" failed"));
    return InputCommand::None;
  }
  //Serial.println(F(" done"));

  InputCommand command = InputCommand::None;

  // message
  aJsonObject* item = aJson.getObjectItem(root, "message");
  if (item && messagePlayer.readJsonObject(item)) {
    command = InputCommand::ShowCurrentMessage;
  }

  // brightness
  item = aJson.getObjectItem(root, "brightness");
  if (item && item->type == aJson_Int) {
    brightness = item->valueint;
    boundBrightness();
    matrix.setBrightness(brightness);
    saveBrightnessSetting();
    command = InputCommand::None;
  }

  // background brightness
  item = aJson.getObjectItem(root, "backgroundBrightness");
  if (item && item->type == aJson_Int) {
    backgroundBrightness = item->valueint;
    boundBackgroundBrightness();
    backgroundLayer.setBrightness(backgroundBrightness);
    saveBackgroundBrightnessSetting();
    command = InputCommand::None;
  }

  // palette
  item = aJson.getObjectItem(root, "palette");
  if (item && item->type == aJson_String) {
    effects.setPalette(item->valuestring);
    command = InputCommand::None;
  }

  // pattern
  item = aJson.getObjectItem(root, "pattern");
  if (item && item->type == aJson_String) {
    //Serial.print(F("Loading pattern "));
    //Serial.println(item->valuestring);
    if (setPattern(item->valuestring))
      command = InputCommand::Update;
    else
      command = InputCommand::None;
  }
  else if (item && item->type == aJson_Int) {
    //Serial.print(F("Loading pattern "));
    //Serial.println(item->valueint);
    if (setPattern(item->valueint))
      command = InputCommand::Update;
    else
      command = InputCommand::None;
  }

  // animation
  item = aJson.getObjectItem(root, "animation");
  if (item && item->type == aJson_String) {
    //Serial.print(F("Loading animation "));
    //Serial.println(item->valuestring);
    if (setAnimation(item->valuestring))
      command = InputCommand::Update;
    else
      command = InputCommand::None;
  }
  else if (item && item->type == aJson_Int) {
    //Serial.print(F("Loading animation "));
    //Serial.println(item->valueint);
    if (setAnimation(item->valueint))
      command = InputCommand::Update;
    else
      command = InputCommand::None;
  }

  // temperature
  item = aJson.getObjectItem(root, "temperature");
  if (item && item->type == aJson_Int) {
    setTemperature(item->valueint);
    command = InputCommand::None;
  }

  // weatherType
  item = aJson.getObjectItem(root, "weatherType");
  if (item && item->type == aJson_Int) {
    setWeatherType(item->valueint);
    command = InputCommand::None;
  }

  // createFile
  item = aJson.getObjectItem(root, "createFile");
  if (item) {
    createFile(item);
    command = InputCommand::None;
  }

  // add support for more specialized items here...

  // fall back on basic command support
  item = aJson.getObjectItem(root, "command");
  if (item && item->type == aJson_String) {
    // custom commands
    if ((String) item->valuestring == "ListAnimations") {
      listAnimations();
      command = InputCommand::None;
    }
    else if ((String) item->valuestring == "ListPatterns") {
      listPatterns();
      command = InputCommand::None;
    }
    else if ((String) item->valuestring == "ListPalettes") {
      effects.listPalettes();
      command = InputCommand::None;
    }

    command = getCommand(item->valuestring);
  }

  aJson.deleteItem(root);

  return command;
}

bool ignoreResetPin = false;

InputCommand readHardwareCommand() {
#ifdef RESET_PIN
  if (digitalRead(RESET_PIN) == LOW) {
    if (!ignoreResetPin) {
      ignoreResetPin = true;
      return InputCommand::Power;
    }
  }
  else {
    ignoreResetPin = false;
  }
#endif

  return InputCommand::None;
}

InputCommand readCommand() {
  InputCommand command = InputCommand::None;

  command = readHardwareCommand();

  if (command == InputCommand::None)
    command = readSerialCommand();

  return command;
}

#endif
