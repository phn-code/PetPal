#include "boxes.h"

#include "../scene.h"
// initialising window
void mini_game::initialise_window() {
  game_video_mode.height = 1080;
  game_video_mode.width = 1920;
  game_window = new sf::RenderWindow(
      game_video_mode, "PetPal",
      sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
  game_window->setFramerateLimit(60);
  showWholeScene(*game_window);
}

void mini_game::initialise_image() {
  mini_game_texture.loadFromFile("assets/minigames/boxes-character.png");
  slider_knob.setTexture(mini_game_texture);
  slider_knob.setScale(
      98.6f / mini_game_texture.getSize().x,
      (98.6f * static_cast<float>(mini_game_texture.getSize().y) /
       mini_game_texture.getSize().x) /
          mini_game_texture.getSize().y);  // Scaling the image
  slider_knob.setPosition(810, 700);
}

void mini_game::initialise_variables() {
  // initialising the variables
  game_window = nullptr;
  min_height_boxes = 50.f;
  max_height_boxes = 200.f;
  maximum_boxes = 100;
  box_spawn_timer_max = 10.f;
  box_spawn_timer = box_spawn_timer_max;
  boxes_one_time = 1;
  is_dragging = false;
  game_over = false;
  points = 0;
  box_falling_speed = 3.0f;
  money_earned = 0;
  mini_game_song_paths = {"assets/audio/songs/call-me-maybe.mp3", "assets/audio/songs/cheap-thrills.mp3",
                          "assets/audio/songs/counting-stars.mp3",
                          "assets/audio/songs/work-from-home.mp3"};
  in_intro_game = true;
  is_game_over_sound_playing = false;
}

mini_game::mini_game() {
  // constructor

  // initialise stuff
  initialise_variables();
  initialise_window();
  initialise_boxes();
  initialise_image();
  initialise_slider();
  initialise_fonts();
  initialise_music();
  initialise_game_over_text();
  initialise_intro_text();
}

mini_game::~mini_game() {
  // destructor
  delete game_window;
}

void mini_game::initialise_boxes() {
  // to initialise the boxes and setting the properties
  falling_boxes.setPosition(
      10.f, 10.f);  // setting position of the boxes to start from 10,10
  random_height_boxes =
      min_height_boxes +
      static_cast<float>(std::rand()) / RAND_MAX *
          (max_height_boxes - min_height_boxes);  // to calculate random height
  falling_boxes.setSize(sf::Vector2f(50.f, random_height_boxes));
  falling_boxes.setFillColor(sf::Color::Red);
  falling_boxes.setOutlineColor(sf::Color::White);
  falling_boxes.setOutlineThickness(1.f);
}

bool mini_game::window_open() { return game_window->isOpen(); }

void mini_game::poll_events() {
  // to do some actions depending upon what the user inputted
  while (game_window->pollEvent(ev)) {
    switch (ev.type) {
      case (sf::Event::Closed):
        game_window->close();
        break;
      case (sf::Event::KeyPressed):
        if (ev.key.code == sf::Keyboard::Escape) {
          game_window->close();
        }
        if (in_intro_game && ev.key.code == sf::Keyboard::Enter) {
          in_intro_game = false;  // Exit intro screen on Enter key press
        }
        break;
      // to check for the dragging one i.e. if the mouse position is on the knob
      // and is clicked upon it will set the is_dragging boolean to truew
      case (sf::Event::MouseButtonPressed):
        if (ev.mouseButton.button == sf::Mouse::Left) {
          if (slider_knob.getGlobalBounds().contains(
                  toScene(*game_window, ev.mouseButton.x, ev.mouseButton.y))) {
            is_dragging = true;
          }
        }
        break;
      case (sf::Event::MouseButtonReleased):
        if (ev.mouseButton.button == sf::Mouse::Left) {
          is_dragging = false;
        }
        break;
      default:
        break;
    }
  }
}

void mini_game::render() {
  // draws and represents everything on the screen
  game_window->clear(sf::Color(230, 230, 220));
  if (in_intro_game) {
    render_intro_text(*game_window);
  } else if (game_over == false) {
    render_boxes(*game_window);
    game_window->draw(slider_track);
    game_window->draw(slider_knob);
    render_text(*game_window);
  } else {
    mini_game_music.pause();
    game_window->clear(sf::Color(230, 230, 220));
    render_game_over_text(*game_window);
  }
  game_window->display();
}

void mini_game::spawn_boxes() {
  // spawns boxes and set their colors and positions
  float random_height_boxes =
      min_height_boxes +
      static_cast<float>(std::rand()) / RAND_MAX *
          (max_height_boxes - min_height_boxes);  // to calculate random height
  falling_boxes.setSize(sf::Vector2f(50.f, random_height_boxes));
  falling_boxes.setPosition(
      static_cast<float>(rand() % static_cast<int>(kScene.x - falling_boxes.getSize().x)),
      0.0);
  falling_boxes.setFillColor(sf::Color(0, 0, 128));
  boxes.push_back(falling_boxes);
}

void mini_game::render_boxes(sf::RenderTarget& target) {
  for (auto& i : boxes) {
    target.draw(i);
  }
}

void mini_game::update() {
  poll_events();
  if (in_intro_game == false) {
    update_boxes();
    update_slider();
    update_points();
    update_text();
    update_game_over_text();
  }
}

void mini_game::update_boxes() {
  if (boxes.size() < boxes_one_time) {
    if (box_spawn_timer >= box_spawn_timer_max) {
      spawn_boxes();
      box_spawn_timer = 0.f;
    } else {
      box_spawn_timer += 1.f;
    }
  }

  for (auto& e : boxes) {
    e.move(0.f, box_falling_speed);
    // checking if the box has touch the slider
    if (e.getPosition().y + e.getSize().y >= slider_track.getPosition().y) {
      game_over = true;
      initialise_game_over_sound();
    }
  }

  // removing boxes that went out of the screen
}

void mini_game::initialise_slider() {
  slider_track.setSize(sf::Vector2f(
      sf::Vector2f(kScene.x, 5.0f)));
  slider_track.setPosition(0, 762);
  slider_track.setFillColor(sf::Color::Black);
}

void mini_game::update_slider() {
  if (is_dragging) {
    sf::Vector2i mouse = sf::Mouse::getPosition(*game_window);
    float position_of_mouse = toScene(*game_window, mouse.x, mouse.y).x;
    float position_of_knob =
        position_of_mouse - (slider_knob.getGlobalBounds().width / 2);
    position_of_knob = std::max(
        slider_track.getPosition().x,
        std::min(position_of_knob, slider_track.getPosition().x +
                                       slider_track.getSize().x -
                                       slider_knob.getGlobalBounds().width));
    // setting the postiion of the slider knob
    slider_knob.setPosition(position_of_knob, slider_knob.getPosition().y);
  }
}

void mini_game::update_points() {
  for (int i = 0; i < boxes.size(); i++) {
    if (slider_knob.getGlobalBounds().intersects(boxes[i].getGlobalBounds())) {
      // getting the height of the box falling
      float individual_height = boxes[i].getSize().y;
      if (individual_height < 60.0f) {
        points =
            points + 6;  // increasing points according to the size of the boxes
      } else if (individual_height < 80.0f) {
        points = points + 5;
      } else if (individual_height < 100.0f) {
        points = points + 4;
      } else if (individual_height < 120.0f) {
        points = points + 3;
      } else if (individual_height < 150.0f) {
        points = points + 2;
      } else if (individual_height < 200.0f) {
        points = points + 1;
      }
      boxes.erase(boxes.begin() + i);
      box_falling_speed += 0.5f;
      i--;  // because we remove one box...from the vector we need to deccrease
            // the iterating counter
    }
  }
}
void mini_game::initialise_fonts() {
  if (!game_font.loadFromFile("assets/fonts/Regular.ttf")) {
    cout << "failed to load" << endl;
  }
  game_ui_text.setFont(game_font);
  game_ui_text.setCharacterSize(65);
  game_ui_text.setFillColor(sf::Color::Black);
  game_ui_text.setPosition(10, kScene.y - 70);
  game_ui_text.setString("NONE");
}

void mini_game::update_text() {
  std::stringstream ss;
  ss << "Points: " << points << "\n\n";
  game_ui_text.setString(ss.str());
}

void mini_game::render_text(sf::RenderTarget& target) {
  target.draw(this->game_ui_text);
}

void mini_game::initialise_music() {
  mini_game_music.openFromFile(get_random_song(mini_game_song_paths));
  mini_game_music.setLoop(true);
  mini_game_music.setVolume(50);
  mini_game_music.play();
}

string mini_game::get_random_song(vector<string>& songs_path) {
  int random_index = rand() % songs_path.size();
  return songs_path[random_index];
}

void mini_game::initialise_game_over_text() {
  game_over_text.setFont(game_font);
  game_over_text.setCharacterSize(80);
  game_over_text.setFillColor(sf::Color::Black);
  game_over_text.setPosition(550, 300);
  game_over_text.setString("NONE");
}

void mini_game::update_game_over_text() {
  std::stringstream ss;
  money_earned = points * 5;
  ss << "  GAME OVER!!" << "\n\n"
     << "Points Scored: " << points << "\n"
     << "Coins earned: " << money_earned << "\n\n"
     << "Press esc to Exit" << "\n\n";
  game_over_text.setString(ss.str());
}

void mini_game::render_game_over_text(sf::RenderTarget& target) {
  target.draw(game_over_text);
}

void mini_game::initialise_intro_text() {
  intro_text.setFont(game_font);
  intro_text.setCharacterSize(50);
  intro_text.setFillColor(sf::Color::Black);
  intro_text.setPosition(100, 100);
  intro_text.setString(
      "Welcome to PetPal!\n\n"
      "Instructions:\n\n"
      "1. Use the slider to catch the falling boxes.\n\n"
      "2. Different box sizes give different points.\n\n"
      "3. Avoid letting the boxes touch the ground!\n\n"
      "4. Press Enter to start the game.\n\n"
      "5. Press Escape at any time to exit.\n\n\n"
      "Good luck!");
}

void mini_game::render_intro_text(sf::RenderTarget& target) {
  target.draw(intro_text);
}
void mini_game::initialise_game_over_sound() {
  if (!is_game_over_sound_playing) {
    game_over_sound.openFromFile("assets/audio/boxes-game-over.mp3");
    game_over_sound.setVolume(100);
    game_over_sound.play();
    is_game_over_sound_playing = true;
  }
}

int mini_game::money_earned_end() { return money_earned; }