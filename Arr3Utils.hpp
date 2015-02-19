#ifndef ARR3_UTILS_HPP
#define ARR3_UTILS_HPP
#include <type_traits>

namespace Diggler {

template <typename T> class Arr3 {
private:
	T*** array;
	int x, y, z;
public:
	Arr3(int x, int y, int z) : x(x), y(y), z(z) {
		array = new T**[x];
		for (int i = 0; i < x; i++) {
			array[i] = new T*[y];
			for (int j = 0; j < y; j++) {
				array[i][j] = new T[z];
/*				std::enable_if<std::is_pointer<T>::value> {
					for (int k = 0; k < z; k++) {
						array[i][j][k] = nullptr;
					}
				}*/
			}
		}
	}
	~Arr3() {
		for(int i = 0; i < x; i++) {
			for(int j = 0; j < y; j++) {
/*				std::enable_if<std::is_pointer<T>::value> {
					for(int k = 0; k < z; k++) {
						if (array[x][y][z]) {
							delete array[x][y][z];
						}
					}
				}*/
				delete[] array[i][j];
			}
			delete[] array[i];
		}
		delete[] array;
	}
	T** operator[](std::size_t idx) const {
		return array[idx];
	}
	int getX() const { return x; }
	int getY() const { return y; }
	int getZ() const { return z; }
	bool sizeEqual(const Arr3 &r) const { return x == r.x && y == r.y && z == r.z; }
};

typedef Arr3<float> Arr3f;
typedef Arr3<int> Arr3i;

}

#endif