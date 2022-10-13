#include <SFML/Graphics.hpp>

#include <array>


// Main application class
class Application {
private:
	sf::RenderWindow* m_window;
	size_t m_windowSize;
	sf::Clock m_clock;
	sf::Font* m_font;

	std::array<std::pair<size_t, bool>, 9 * 9> m_sudoku;
	bool m_selected = false;
	sf::Vector2u m_selectedCell{ };

	bool m_solving = false;
	std::array<size_t, 9 * 9> m_numbers{ };

	float timer = 0.0f;
	const float tickSpeed = 0.f;

private:
	// Initializes the applicaiton
	void initialize();

	// Returns true if the number can be entered in the given cell
	bool validateCell(size_t number, const sf::Vector2u& index);
	// Solves the sudoku puzzle one step at a time
	bool solveSudoku();
	// Renders the sudoku grid
	void renderGrid();
	// Renders the numbers on the grid
	void renderNumbers();

	// Handles any user input
	void handleInput();
	// Updates the application
	void update();
	// Renders anything onto the window
	void render();

public:
	// Constructor
	Application();
	// Destructor
	~Application();

	// Runs the application
	void run();
};


Application::Application()
	: m_window{ nullptr },
	m_font{ nullptr },
	m_sudoku{ } {
	initialize();
}


Application::~Application() {
	delete m_window;
}


void Application::initialize() {
	// Initializes the window
	m_windowSize = 900;
	sf::VideoMode windowVideoMode = sf::VideoMode{ m_windowSize, m_windowSize };
	sf::String windowTitle = "Sudoku Solver";
	size_t windowStyle = sf::Style::Close;
	m_window = new sf::RenderWindow{ windowVideoMode, windowTitle, windowStyle };

	// Initializes the font
	m_font = new sf::Font;
	m_font->loadFromFile("cour.ttf");
}


bool Application::validateCell(size_t number, const sf::Vector2u& index) {
	// Checks the row and column
	for (size_t i = 0; i < 9; ++i) {
		if (m_sudoku.at(i + index.y * 9).first == number) return false;
		if (m_sudoku.at(index.x + i * 9).first == number) return false;
	}

	// Checks the 3x3 subgrid
	for (size_t i = (index.x / 3) * 3; i < (index.x / 3) * 3 + 3; ++i) {
		for (size_t j = (index.y / 3) * 3; j < (index.y / 3) * 3 + 3; ++j) {
			if (m_sudoku.at(i + j * 9).first == number) return false;
		}
	}

	return true;
}


bool Application::solveSudoku() {
	// Tracks the progress made into the sudoku puzzle
	static size_t index = 0;

	// Returns true and resets index if sudoku is solved
	if (index >= 81) return true;

	// If there is a user entered number at the index, then continue
	if (m_sudoku.at(index).second) {
		index++;
		return false;
	}

	// Looks for a number that can work
	bool numFound = false;
	size_t num = m_numbers.at(index) ? m_numbers.at(index) : 1;
	for (; num <= 9; ++num) {
		if (validateCell(num, { index % 9, index / 9 })) {
			numFound = true;
			break;
		}
	}

	// If a number that works has been found
	if (numFound) {
		m_sudoku.at(index).first = num;
		m_numbers.at(index) = num;
		index++;
		return false;
	}

	// If no suitable number is found, backtrack
	m_sudoku.at(index).first = 0;
	m_numbers.at(index) = 0;
	index--;
	while (m_sudoku.at(index).second) index--;
	return false;
}


void Application::renderGrid() {
	// Vertex array for the lines
	sf::VertexArray lines{ sf::Quads };
	sf::Color thinColor = { 100, 100, 100 };
	sf::Color thickColor = sf::Color::Black;

	// Calculates the thin lines
	for (float i = 0.0f; i <= m_windowSize; i += m_windowSize / 9.0f) {
		lines.append({ { i - 2.0f, 0.0f }, thinColor });
		lines.append({ { i + 2.0f, 0.0f }, thinColor });
		lines.append({ { i + 2.0f, static_cast<float>(m_windowSize) }, thinColor });
		lines.append({ { i - 2.0f, static_cast<float>(m_windowSize) }, thinColor });

		lines.append({ { 0.0f, i - 2.0f }, thinColor });
		lines.append({ { 0.0f, i + 2.0f }, thinColor });
		lines.append({ { static_cast<float>(m_windowSize), i + 2.0f }, thinColor });
		lines.append({ { static_cast<float>(m_windowSize), i - 2.0f }, thinColor });
	}

	// Calculates the thick lines
	for (float i = 0.0f; i <= m_windowSize; i += m_windowSize / 3.0f) {
		lines.append({ { i - 3.0f, 0.0f }, thickColor });
		lines.append({ { i + 3.0f, 0.0f }, thickColor });
		lines.append({ { i + 3.0f, static_cast<float>(m_windowSize) }, thickColor });
		lines.append({ { i - 3.0f, static_cast<float>(m_windowSize) }, thickColor });

		lines.append({ { 0.0f, i - 3.0f }, thickColor });
		lines.append({ { 0.0f, i + 3.0f }, thickColor });
		lines.append({ { static_cast<float>(m_windowSize), i + 3.0f }, thickColor });
		lines.append({ { static_cast<float>(m_windowSize), i - 3.0f }, thickColor });
	}

	m_window->draw(lines);
}


void Application::renderNumbers() {
	// Iterates over all the number in the sudoku
	for (size_t i = 0; i < 9; ++i) {
		for (size_t j = 0; j < 9; ++j) {
			// If there is no number there, then continue
			if (!m_sudoku.at(i + j * 9).first) continue;

			// Calculates the position of the text
			sf::Text text{ std::to_string(m_sudoku.at(i + j * 9).first), *m_font, 80 };
			sf::FloatRect textRect = text.getLocalBounds();
			text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			text.setPosition(i * m_windowSize / 9 + m_windowSize / 18, j * m_windowSize / 9 + m_windowSize / 18);
			text.setStyle(sf::Text::Bold);

			// Colors the text red if entered by the user
			if (m_sudoku.at(i + j * 9).second) text.setFillColor(sf::Color::Black);
			else                               text.setFillColor(sf::Color::Red);

			// Renders the text
			m_window->draw(text);
		}
	}
}


void Application::handleInput() {
	// Event loop
	sf::Event sfmlEvent;
	while (m_window->pollEvent(sfmlEvent)) {
		switch (sfmlEvent.type) {
		// If the user pressed the quit button
		case sf::Event::Closed:
			m_window->close();
			break;

		// If the user clicked with mouse
		case sf::Event::MouseButtonPressed:
			// Breaks if the button isn't the left mouse button
			if (sfmlEvent.mouseButton.button != sf::Mouse::Left) break;

			// Calculates the 
			m_selectedCell = sf::Vector2u{ sf::Mouse::getPosition(*m_window) } / (m_windowSize / 9);
			m_selected = true;

		// If the user typed with the keyboard
		case sf::Event::KeyPressed:
			// Does nothing if no cell is selected
			if (!m_selected) break;

			// If the user entered a number
			if (sfmlEvent.key.code >= 27 && sfmlEvent.key.code <= 35) {
				if (!validateCell(sfmlEvent.key.code - 26, m_selectedCell)) break;
				m_sudoku.at(m_selectedCell.x + m_selectedCell.y * 9) = std::make_pair(sfmlEvent.key.code - 26, true);
			}

			// If the user wants to move around using arrow keys
			else if (sfmlEvent.key.code == sf::Keyboard::Key::BackSpace) {
				m_sudoku.at(m_selectedCell.x + m_selectedCell.y * 9).first = 0;
			} else if (sfmlEvent.key.code == sf::Keyboard::Key::Up) {
				if (m_selectedCell.y != 0) m_selectedCell.y--;
			} else if (sfmlEvent.key.code == sf::Keyboard::Key::Down) {
				if (m_selectedCell.y != 8) m_selectedCell.y++;
			} else if (sfmlEvent.key.code == sf::Keyboard::Key::Left) {
				if (m_selectedCell.x != 0) m_selectedCell.x--;
			} else if (sfmlEvent.key.code == sf::Keyboard::Key::Right) {
				if (m_selectedCell.x != 8) m_selectedCell.x++;
			}

			// If the user pressed enter, start solving
			else if (sfmlEvent.key.code == sf::Keyboard::Key::Enter) {
				m_solving = true;
			}

			break;
		}
	}
}


void Application::update() {
	// Calculates the elapsed time between frames
	float deltaTime = m_clock.restart().asSeconds();
	timer += deltaTime;
	if (timer >= tickSpeed) {
		timer = 0.0f;

		if (m_solving) m_solving = !solveSudoku();
	}
}


void Application::render() {
	// Clears the window
	m_window->clear(sf::Color::White);

	// Renders the grid and the numbers
	renderGrid();
	renderNumbers();

	// Highlights the selected cell
	if (m_selected) {
		sf::RectangleShape selectedRect{ sf::Vector2f{ m_window->getSize() } / 9.0f };
		selectedRect.setPosition(m_selectedCell.x * selectedRect.getSize().x,
			m_selectedCell.y * selectedRect.getSize().y);
		selectedRect.setFillColor(sf::Color{ 0, 255, 0, 100 });
		
		m_window->draw(selectedRect);
	}

	// Displays the current frame
	m_window->display();
}


void Application::run() {
	// Main game loop
	while (m_window->isOpen()) {
		handleInput();
		update();
		render();
	}
}


// Main function
int main() {
	Application app;
	app.run();

	return 0;
}
