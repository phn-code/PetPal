#pragma once
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdlib>  //for random function
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
class mini_game {
 private:
  sf::RenderWindow* game_window;
  sf::Event ev;
  sf::VideoMode game_video_mode;
  vector<sf::RectangleShape> boxes;
  sf::RectangleShape falling_boxes;

  // variables for boxes
  int maximum_boxes;   // how many boxes can the game generate in total
  int boxes_one_time;  // how many boxes on the screen at once
  float min_height_boxes;
  float max_height_boxes;
  float box_spawn_timer;
  float box_spawn_timer_max;
  float box_falling_speed;
  float random_height_boxes;

  // for the image
  sf::Texture mini_game_texture;

  // for the slider variable
  sf::RectangleShape slider_track;
  sf::Sprite slider_knob;
  bool is_dragging;

  // points
  bool game_over;
  unsigned points;
  int money_earned;
  // fonts and texts
  sf::Font game_font;
  sf::Text game_ui_text;

  // sound
  sf::Music mini_game_music;

  // vector of songs
  vector<string> mini_game_song_paths;

  // game over screen
  sf::Text game_over_text;

  // game over sound
  sf::Music game_over_sound;
  bool is_game_over_sound_playing;

  // intro screen
  bool in_intro_game;   // checks if the game has just started and is in intro
                        // mode
  sf::Text intro_text;  // to display the instructions

  // private functions
  void initialise_variables();
  void initialise_window();

 public:
  mini_game();   // comstructor
  ~mini_game();  // destructor
  void initialise_boxes();
  bool window_open();
  void render();  // renders for the whole game
  void update();  // updates the screen for the whole game
  void poll_events();
  void spawn_boxes();
  void render_boxes(sf::RenderTarget& target);  // renders the boxes
  void update_boxes();
  void initialise_image();
  void initialise_slider();
  void update_slider();
  void update_points();
  void initialise_fonts();
  void update_text();  // updates the text in the main game
  void render_text(sf::RenderTarget& target);  // renders text in the main game
  void initialise_music();
  string get_random_song(vector<string>& songs_path);
  void initialise_game_over_text();
  void update_game_over_text();
  void render_game_over_text(sf::RenderTarget& target);
  void initialise_intro_text();
  void render_intro_text(sf::RenderTarget& target);
  void initialise_game_over_sound();
  int money_earned_end();
};