#include "JoystickDirections.h"

const std::pair<J_DIRECTION, String> J_DIRECTION_PAIR[] = {
  {J_DIRECTION::CENTER, "CENTER"},
  {J_DIRECTION::UP, "UP"},
  {J_DIRECTION::DOWN, "DOWN"},
  {J_DIRECTION::LEFT, "LEFT"},
  {J_DIRECTION::RIGHT, "RIGHT"},
  {J_DIRECTION::UP_LEFT, "UP_LEFT"},
  {J_DIRECTION::UP_RIGHT, "UP_RIGHT"},
  {J_DIRECTION::DOWN_LEFT, "DOWN_LEFT"},
  {J_DIRECTION::DOWN_RIGHT, "DOWN_RIGHT"},
  {J_DIRECTION::CENTER_BUTTON, "CENTER_BUTTON"},
  {J_DIRECTION::UP_BUTTON, "UP_BUTTON"},
  {J_DIRECTION::DOWN_BUTTON, "DOWN_BUTTON"},
  {J_DIRECTION::LEFT_BUTTON, "LEFT_BUTTON"},
  {J_DIRECTION::RIGHT_BUTTON, "RIGHT_BUTTON"},
  {J_DIRECTION::UP_LEFT_BUTTON, "UP_LEFT BUTTON"},
  {J_DIRECTION::UP_RIGHT_BUTTON, "UP_RIGHT_BUTTON"},
  {J_DIRECTION::DOWN_LEFT_BUTTON, "DOWN_LEFT_BUTTON"},
  {J_DIRECTION::DOWN_RIGHT_BUTTON, "DOWN_RIGHT_BUTTON"}
};

J_DIRECTION mapStringToJdirections(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : J_DIRECTION_PAIR)
  {
      if (entry.second == inputString)
      {
          return entry.first;
      }
  }
  // Return a special value or throw an exception if the string is not found
  return J_DIRECTION::CENTER;
}

String mapJdirectionsToString(J_DIRECTION input)
{
  for (auto &pair : J_DIRECTION_PAIR) {
    if (pair.first == input) {
        return pair.second;
    }
  }
  return "CENTER";
}
