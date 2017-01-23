//
// Created by cpasjuste on 11/01/17.
//

#include <SFML/Window/Event.hpp>
#include "sfml_input.h"

static int key_id[Input::Key::KEY_COUNT]{
        Input::Key::KEY_UP,
        Input::Key::KEY_DOWN,
        Input::Key::KEY_LEFT,
        Input::Key::KEY_RIGHT,
        Input::Key::KEY_COIN,
        Input::Key::KEY_START,
        Input::Key::KEY_FIRE1,
        Input::Key::KEY_FIRE2,
        Input::Key::KEY_FIRE3,
        Input::Key::KEY_FIRE4,
        Input::Key::KEY_FIRE5,
        Input::Key::KEY_FIRE6
};

SFMLInput::SFMLInput(SFMLRenderer *rdr) {

    renderer = rdr;

    int joy_count = 0;

    for (int i = 0; i < 4; i++) {
        if (sf::Joystick::isConnected(i)) {
            printf("Joystick: %i\n", i);
            players[i].enabled = true;
            players[i].id = i;
            sf::Joystick::Identification id = sf::Joystick::getIdentification(i);
            printf("Buttons %d\n", sf::Joystick::getButtonCount(i));
            joy_count++;
        }
    }

    printf("%d Joystick(s) Found\n", joy_count);

    // allow keyboard mapping to player1
    players[0].enabled = true;

    for (int i = 0; i < PLAYER_COUNT; i++) {
        for (int k = 0; k < KEY_COUNT; k++) {
            players[i].mapping[k] = 0;
        }
    }

    for (int i = 0; i < KEY_COUNT; i++) {
        keyboard.mapping[i] = 0;
    }
}

SFMLInput::~SFMLInput() {

    for (int i = 0; i < PLAYER_COUNT; i++) {
        players[i].enabled = false;
        players[i].id = -1;
    }
}

int SFMLInput::GetButton(int player) {
    sf::Event event;
    while (renderer->window.pollEvent(event)) {
        if (event.type == sf::Event::JoystickButtonPressed) {
            if (event.joystickButton.joystickId == player) {
                return event.joystickButton.button;
            }
        }
    }
    return -1;
}

Input::Player *SFMLInput::Update(bool rotate) {

    for (int i = 0; i < PLAYER_COUNT; i++) {
        players[i].state = 0;
    }

    sf::Event event;
    while (renderer->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            players[0].state |= KEY_QUIT;
            return players;
        }
        if(event.type == sf::Event::KeyPressed) {
            printf("%i\n", (int)event.key.code);
        }
        if(event.type == sf::Event::JoystickButtonPressed) {
            printf("%i\n", (int)event.joystickButton.button);
        }
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {

        if (!players[i].enabled) {
            continue;
        }

        // hat
        process_hat(players[i], rotate);

        // sticks
        process_axis(players[i], rotate);

        // buttons
        process_buttons(players[i], rotate);
    }

    // keyboard
    process_keyboard(players[0], rotate);

    return players;
}

void SFMLInput::process_axis(Input::Player &player, bool rotate) {

    if (!player.enabled) {
        return;
    }

    // X AXIS
    if (sf::Joystick::hasAxis(player.id, sf::Joystick::X)) {
        int x = (int) sf::Joystick::getAxisPosition(player.id, sf::Joystick::X) * 320;
        if (x > player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_DOWN : Input::Key::KEY_RIGHT;
        } else if (x < -player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_UP : Input::Key::KEY_LEFT;
        }
    }

    // Y AXIS
    if (sf::Joystick::hasAxis(player.id, sf::Joystick::Y)) {
        int y = (int) sf::Joystick::getAxisPosition(player.id, sf::Joystick::Y) * 320;
        if (y > player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_LEFT : Input::Key::KEY_DOWN;
        } else if (y < -player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_RIGHT : Input::Key::KEY_UP;
        }
    }
}

void SFMLInput::process_hat(Input::Player &player, bool rotate) {

    if (!player.enabled) {
        return;
    }

    // X AXIS
    if (sf::Joystick::hasAxis(player.id, sf::Joystick::PovX)) {
        int x = (int) sf::Joystick::getAxisPosition(player.id, sf::Joystick::PovX) * 320;
        if (x > player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_DOWN : Input::Key::KEY_RIGHT;
        } else if (x < -player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_UP : Input::Key::KEY_LEFT;
        }
    }

    // Y AXIS
    if (sf::Joystick::hasAxis(player.id, sf::Joystick::PovY)) {
        int y = (int) sf::Joystick::getAxisPosition(player.id, sf::Joystick::PovY) * 320;
        if (y > player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_LEFT : Input::Key::KEY_DOWN;
        } else if (y < -player.dead_zone) {
            player.state |= rotate ? Input::Key::KEY_RIGHT : Input::Key::KEY_UP;
        }
    }

}

void SFMLInput::process_buttons(Input::Player &player, bool rotate) {

    if (!player.enabled) {
        return;
    }

    for (int i = 0; i < KEY_COUNT; i++) {

        int mapping = player.mapping[i];
        if(mapping < 0)
            mapping = 0;

        if (sf::Joystick::isButtonPressed(player.id, mapping)) {
            if (rotate && key_id[i] == Input::Key::KEY_UP) {
                player.state |= Input::Key::KEY_RIGHT;
            } else if (rotate && key_id[i] == Input::Key::KEY_DOWN) {
                player.state |= Input::Key::KEY_LEFT;
            } else if (rotate && key_id[i] == Input::Key::KEY_LEFT) {
                player.state |= Input::Key::KEY_UP;
            } else if (rotate && key_id[i] == Input::Key::KEY_RIGHT) {
                player.state |= Input::Key::KEY_DOWN;
            } else {
                player.state |= key_id[i];
            }
        }
    }
}

void SFMLInput::process_keyboard(Input::Player &player, bool rotate) {

    for (int i = 0; i < KEY_COUNT; i++) {
        sf::Keyboard::Key key ((sf::Keyboard::Key)keyboard.mapping[i]);
        if (sf::Keyboard::isKeyPressed(key)) {
            if (rotate && key_id[i] == Input::Key::KEY_UP) {
                player.state |= Input::Key::KEY_RIGHT;
            } else if (rotate && key_id[i] == Input::Key::KEY_DOWN) {
                player.state |= Input::Key::KEY_LEFT;
            } else if (rotate && key_id[i] == Input::Key::KEY_LEFT) {
                player.state |= Input::Key::KEY_UP;
            } else if (rotate && key_id[i] == Input::Key::KEY_RIGHT) {
                player.state |= Input::Key::KEY_DOWN;
            } else {
                player.state |= key_id[i];
            }
        }
    }
}
