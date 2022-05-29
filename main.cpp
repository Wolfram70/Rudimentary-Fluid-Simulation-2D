#include<iostream>
#include<conio.h>
#include <math.h>
#include<SFML\Graphics.hpp>
#include<SFML\Window.hpp>

int N = 100;

void boundary(float* x1, float* y1, int n)
{
	for (int j = 0; j < n; j++)
	{
		if (x1[1 + n * j] < 0)
		{
			x1[1 + n * j] = -x1[1 + n * j];
		}

		if (y1[j + n * 1] < 0)
		{
			y1[j + n * 1] = -y1[j + n * 1];
		}

		if (x1[n - 2 + n * j] > 0)
		{
			x1[n - 2 + n * j] = -x1[n - 2 + n * j];
		}

		if (y1[j + n * (n - 2)] > 0)
		{
			y1[j + n * (n - 2)] = -y1[j + n * (n - 2)];
		}
	}
}

void decompose(float* diff, float* p, float* x1, float* y1, int n)
{
	for (int i = 1; i < n - 1; i++)
	{
		for (int j = 1; j < n - 1; j++)
		{
			diff[i + n * j] = (x1[i + 1 + n * j] - x1[i - 1 + n * j] +
				              y1[i + n * (j + 1)] - y1[i + n * (j - 1)]) / (n * 2);
			diff[i + n * j] = -diff[i + n * j];
			p[i + n * j] = 0;
        }
	}

	for (int k = 0; k < 15; k++)
	{
		for (int i = 1; i < n - 1; i++)
		{
			for (int j = 1; j < n - 1; j++)
			{
				p[i + n * j] = (p[i + 1 + n * j] + p[i - 1 + n * j] +
					p[i + n * (j + 1)] + p[i + n * (j - 1)] + diff[i + n * j]) / 4;
			}
		}
	}

	for (int i = 1; i < n - 1; i++)
	{
		for (int j = 1; j < n - 1; j++)
		{
			x1[i + n * j] -= ((p[i + 1 + n * j] - p[i - 1 + n * j]) * n) / 2;
			y1[i + n * j] -= ((p[i + n * (j + 1)] - p[i + n * (j - 1)]) * n) / 2;
		}
	}
}

void advect(float* d0, float* d1, float* x1, float* y1, int n, float dt)
{
	float xf, yf;
	int i0, i1, j0, j1;

	for (int i = 1; i < n - 1; i++)
	{
		for (int j = 1; j < n - 1; j++)
		{
			xf = i - x1[i + n * j] * dt;
			yf = j - y1[i + n * j] * dt;
			if (xf < 0.5) xf = 0.5;
			if (yf < 0.5) yf = 0.5;
			if (xf > n - 0.5) xf = n - 0.5;
			if (yf > n - 0.5) yf = n - 0.5;
			i0 = (int)xf;
			i1 = i0 + 1;
			j0 = (int)yf;
			j1 = j0 + 1;
			d1[i + n * j] = (i1 - xf) * ((j1 - yf) * d0[i0 + n * j0] + (yf - j0) * d0[i0 + n * j1]) + (xf - i0) * ((j1 - yf) * d0[i1 + n * j0] + (yf - j0) * d0[i1 + n * j1]);
		}
	}
}

void swap(float** a, float** b)
{
	float* tempy;
	tempy = *a;
	*a = *b;
	*b = tempy;

}

void diffuse(float* d0, float* d1, int n, float k)
{
	for (int i = 0; i < n; i++) 
	{
		for (int j = 0; j < n; j++) 
		{
			d1[i + n * j] = 0;
		}
	}

	for (int l = 0; l < 15; l++)
	{
		for (int j = 1; j < n - 1; j++)
		{
			for (int i = 1; i < n - 1; i++)
			{
				d1[i + n * j] = ( d0[i + n * j] + ((k / 4) * (d1[i - 1 + n * j] + d1[i + 1 + n * j] + d1[i + n * (j - 1)] + d1[i + n * (j + 1)]))) / (k + 1);
			}
		}
	}
}

int main()
{
	int scale = 4;
	float kd = 30;
	float kv = 70;
	float step = 2000;
	float stepvx = 4000;
	float stepvy = stepvx;
	int potato = 110;
	float dt = 0.01;
	float incrd = 100;

	bool mouse = false;

	std::cout << "Enter grid size:";
	std::cin >> N;
	std::cout << "Enter diffusion constant (between 0 to 100):";
	std::cin >> kd;
	std::cout << "Enter measure of viscosity (between 0 to 100):";
	std::cin >> kv;
	std::cout << "Enter dye source step size (between 10000 to 80000):";
    std::cin >> step;
	std::cout << "Enter velocity step size (x) (between 2000 to 20000):";
	std::cin >> stepvx;
	std::cout << "Enter velocity step size (y) (between 2000 to 20000):";
	std::cin >> stepvy;
	std::cout << "Enter scaling factor:";
	std::cin >> scale;
	std::cout << "Enter the exponential scaling factor (between 100 and 200):";
	std::cin >> potato;
	std::cout << "Enter density constant increment (between 1000 to 20000):";
	std::cin >> incrd;
	std::cout << "Enter dt (0 to 1):";
	std::cin >> dt;

	char kakka;
	std::cout << "Do you want a fixed source in the middle of the window (y/n)?";
	kakka = _getch();

    float* d0 = new float[N * N];
	float* d1 = new float[N * N];
	float* x0 = new float[N * N];
	float* y0 = new float[N * N];
	float* x1 = new float[N * N];
	float* y1 = new float[N * N];

	float* diff = new float[N * N];
	float* p = new float[N * N];

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			d0[i + N * j] = 0;
			d1[i + N * j] = 0;
			x0[i + N * j] = 0;
			y0[i + N * j] = 0;
			x1[i + N * j] = 0;
			y1[i + N * j] = 0;
			diff[i + N * j] = 0;
			p[i + N * j] = 0;
		}
	}

	sf::RenderWindow window(sf::VideoMode(scale * N, scale * N), "Diffusion", sf::Style::Default);
	sf::Image buffer;
	buffer.create(scale * N, scale * N, sf::Color(0, 0, 0));

	sf::Texture t;
	t.loadFromImage(buffer);
	sf::Sprite buffersprite(t);

	while (window.isOpen())
	{
        if(kakka == 'y')
		{
			d0[10 + (N * 10)] = step;
			y0[10 + (N * 10)] = -stepvx;
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonReleased) 
			{
				mouse = false;
			}
			
			if (event.type == sf::Event::MouseButtonPressed)
			{
				int a = sf::Mouse::getPosition(window).x;
				int b = sf::Mouse::getPosition(window).y;
				int a1;
				int b1;
				a1 = a / scale;
				b1 = b / scale;
				if ((a > 0) && (a < scale * N) && (b > 0) && (b < scale * N)) {
					d0[a1 + N * b1] += step;
				}
				mouse = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				{
					incrd += 1000;
				}
				else
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
					{
						stepvx += 500;
					}

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
					{
						stepvy += 500;
					}
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace))
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				{
					if(incrd > 1000)
					incrd -= 1000;
				}
				else
				{
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
					{
						stepvx -= 500;
					}

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
					{
						stepvy -= 500;
					}
				}
			}
		}

		if (mouse)
		{
			int a = sf::Mouse::getPosition(window).x;
			int b = sf::Mouse::getPosition(window).y;
			int a1;
			int b1;
			a1 = a / scale;
			b1 = b / scale;
			if ((a > 0) && (a < scale * N) && (b > 0) && (b < scale * N)) {
				d0[a1 + N * b1] += incrd;
				x0[a1 + N * b1] = stepvx;
				y0[a1 + N * b1] = stepvy;
			}
		}

		window.clear();

		for (int i = 1; i < N - 1; i++)
		{
			for (int j = 1; j < N - 1; j++) 
			{
				for (int l = 0; l < scale; l++) 
				{
					for (int m = 0; m < scale; m++) 
					{
						buffer.setPixel(scale * i + l, scale * j + m, sf::Color(255 * pow(1.025, -d0[i + N * j] / potato), 255 * pow(1.025, -d0[i + N * j] / potato), 255, 255));
                    }
				}
			}
		}
		
		t.loadFromImage(buffer);

		window.draw(buffersprite);

		window.display();
		
		diffuse(x0, x1, N, kv * dt);
		diffuse(y0, y1, N, kv * dt);
		boundary(x1, y1, N);
		
		decompose(diff, p, x1, y1, N);
		boundary(x1, y1, N);
		swap(&x0, &x1);
		swap(&y0, &y1);
		
		advect(x0, x1, x0, y0, N, dt);
		advect(y0, y1, x0, y0, N, dt);
		boundary(x1, y1, N);

		decompose(diff, p, x1, y1, N);
		boundary(x1, y1, N);

        diffuse(d0, d1, N, kd * dt);
		swap(&d0, &d1);
		advect(d0, d1, x1, y1, N, dt);

		swap(&d0, &d1);
		swap(&x0, &x1);
		swap(&y0, &y1);
	}

	delete[] d0;
	delete[] d1;
	delete[] x0;
	delete[] x1;
	delete[] y0;
	delete[] y1;
	delete[] diff;
	delete[] p;

	return 0;
}