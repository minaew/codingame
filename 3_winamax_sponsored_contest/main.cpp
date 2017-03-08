#include <iostream>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


//#define WEB

#define CHECKED   'C'
#define USED_HOLE 'h'
#define HOLE      'H'
#define EMPTY     '.'
#define WATER     'X'

/*
. - for an empty cell.
1-9 for a ball. The value indicates the ball's shot count.
X - water hazard.
H - for a hole.
*/


// globals
int width;
int height;
char* field;
std::vector< std::vector<int> > paths; // for recursive function 'find_hole'
std::vector< std::vector<int> > global_layed_paths;


struct Ball {
    Ball(int i_sc, int i_x, int i_y) : shot_count(i_sc), x(i_x), y(i_y) {}

    int shot_count;
    int x;
    int y;
    std::vector< std::vector<int> > paths;
};


bool isContains(const std::vector<int>& v, int val) {
    return v.end() != std::find(v.begin(), v.end(), val);
}

bool isIntersected(const std::vector<int>& p1, const std::vector<int>& p2) {
    // path with all cell included
    std::vector<int> p1_extended;
    // std::vector<int> p2_extended;

    // extend p1
    p1_extended.push_back(p1[0]);
    for (size_t i = 0; i < p1.size()-1; i++) {
        // next coords
        int xn = p1[i] % width;
        int yn = p1[i] / width;
        // previous coords
        int xp = p1[i+1] % width;
        int yp = p1[i+1] / width;
        // step
        // right
        if (xn > xp) for (int x = xp; x < xn; x++) p1_extended.push_back(yp*width+x);
        // left
        if (xn < xp) for (int x = xp; x > xn; x--) p1_extended.push_back(yp*width+x);
        // bottom
        if (yn > yp) for (int y = yp; y < yn; y++) p1_extended.push_back(y*width+xp);
        // up
        if (yn < yp) for (int y = yp; y > yn; y--) p1_extended.push_back(y*width+xp);
    }


    // look for intersections
    if (isContains(p1_extended, p2[0]))
        return true;
    for (size_t i = 0; i < p2.size()-1; i++) {
        // next coords
        int xn = p2[i] % width;
        int yn = p2[i] / width;
        // previous coords
        int xp = p2[i+1] % width;
        int yp = p2[i+1] / width;
        // step
        // right
        if (xn > xp) for (int x = xp; x < xn; x++) if (isContains(p1_extended, yp*width+x)) return true;
        // left
        if (xn < xp) for (int x = xp; x > xn; x--) if (isContains(p1_extended, yp*width+x)) return true;
        // bottom
        if (yn > yp) for (int y = yp; y < yn; y++) if (isContains(p1_extended, y*width+xp)) return true;
        // up
        if (yn < yp) for (int y = yp; y > yn; y--) if (isContains(p1_extended, y*width+xp)) return true;
    }
    return false;
}

// x,y - cell coordinates
bool isInsideField(int x, int y) {
    return (x >= 0) && (y >= 0) && (x < width) && (y < height);
}

// return index of path stop
// -1 if no path with current step
int findHole(int shot_count, int x, int y) {

    int index = y*width+x;

    if (field[index] == WATER) { // water hazard
        return -1;
    }

    if (field[index] == CHECKED) { // visited
        return -1;
    }

    if (field[index] == HOLE) { // hole found
        std::vector<int> init;
        init.push_back(index);
        paths.push_back(init);
        return index;
    }

    if (shot_count == 0) {
        return -1;
    }


    // step in 4 directions
    int r;
    bool path_found = false;
    bool step_is_valid;

    // right
    step_is_valid = false;
    if ( isInsideField(x+shot_count, y) ) {
        step_is_valid = true;
        for (int i = x; i < x+shot_count; i++) {
            int loc_index = y*width+i;
            // path cross ball start position
            if ((field[loc_index] >= '1') && (field[loc_index] <= '9') && (i != x)) {
                step_is_valid = false;
                break;
            }
            // path cross hole
            if (field[loc_index] == HOLE) {
                step_is_valid = false;
                break;
            }
        }
    }
    if (step_is_valid) {
        // mark path cheked
        for (int i = x; i < x+shot_count; i++) {
            int loc_index = y*width+i;
            if (field[loc_index] == EMPTY) {
                field[loc_index] = CHECKED;
            }
        }
        // step
        r = findHole(shot_count-1, x+shot_count, y);
        if (r != -1) {
            path_found = true;
            for (size_t i = 0; i < paths.size(); i++) {
                if ( paths[i].back() == r ) {
                    paths[i].push_back(index);
                }
            }
        }
    }


    // left
    step_is_valid = false;
    if (isInsideField(x-shot_count, y)) {
        step_is_valid = true;
        for (int i = x; i > x-shot_count; i--) {
            int loc_index = y*width+i;
            // path cross ball start position
            if ((field[loc_index] >= '1') && (field[loc_index] <= '9') && (i != x)) {
                step_is_valid = false;
                break;
            }
            // path cross hole
            if (field[loc_index] == HOLE) {
                step_is_valid = false;
                break;
            }
        }
    }
    if (step_is_valid) {
        // mark path cheked
        for (int i = x; i > x-shot_count; i--) {
            int loc_index = y*width+i;
            if (field[loc_index] == EMPTY) {
                field[loc_index] = CHECKED;
            }
        }
        // step
        r = findHole(shot_count-1, x-shot_count, y);
        if (r != -1) {
            path_found = true;
            for (size_t i = 0; i < paths.size(); i++) {
                if ( paths[i].back() == r ) {
                    paths[i].push_back(index);
                }
            }
        }
    }

    // bottom
    step_is_valid = false;
    if (isInsideField(x, y+shot_count)) {
        step_is_valid = true;
        for (int i = y; i < y+shot_count; i++) {
            int loc_index = i*width+x;
            // path cross ball start position
            if ((field[loc_index] >= '1') && (field[loc_index] <= '9') && (i != y)) {
                step_is_valid = false;
                break;
            }
            // path cross hole
            if (field[loc_index] == HOLE) {
                step_is_valid = false;
                break;
            }
        }
    }
    if (step_is_valid) {
        // mark path cheked
        for (int i = y; i < y+shot_count; i++) {
            int loc_index = i*width+x;
            if (field[loc_index] == EMPTY) {
                field[loc_index] = CHECKED;
            }
        }
        // step
        r = findHole(shot_count-1, x, y+shot_count);
        if (r != -1) {
            path_found = true;
            for (size_t i = 0; i < paths.size(); i++) {
                if ( paths[i].back() == r ) {
                    paths[i].push_back(index);
                }
            }
        }
    }

    // top
    step_is_valid = false;
    if (isInsideField(x, y-shot_count)) {
        step_is_valid = true;
        for (int i = y; i > y-shot_count; i--) {
            int loc_index = i*width+x;
            // path cross ball start position
            if ((field[loc_index] >= '1') && (field[loc_index] <= '9') && (i != y)) {
                step_is_valid = false;
                break;
            }
            // path cross hole
            if (field[loc_index] == HOLE) {
                step_is_valid = false;
                break;
            }
        }
    }

    if (step_is_valid) {
        // mark path cheked
        for (int i = y; i > y-shot_count; i--) {
            int loc_index = i*width+x;
            if (field[loc_index] == EMPTY) {
                field[loc_index] = CHECKED;
            }
        }
        // step
        r = findHole(shot_count-1, x, y-shot_count);
        if (r != -1) {
            path_found = true;
            for (size_t i = 0; i < paths.size(); i++) {
                if ( paths[i].back() == r ) {
                    paths[i].push_back(index);
                }
            }
        }
    }

    if (path_found)
        return index;
    else
        return -1;
}


bool layPaths(std::vector<Ball> i_balls,
              std::vector< std::vector<int> > i_layed_paths,
              int i_path_index) {

    // choose i_path_index path
    std::vector<int> path = i_balls[0].paths[i_path_index];
    i_layed_paths.push_back(path);
    // remove firts ball
    i_balls.erase(i_balls.begin());
    if (i_balls.size() == 0) {
        global_layed_paths = i_layed_paths;
        return true;
    }
    // remove paths, intersected with just chosen
    for (size_t i = 0; i < i_balls.size(); i++) {
        i_balls[i].paths.erase(
        std::remove_if(i_balls[i].paths.begin(), i_balls[i].paths.end(),
                       [path](const std::vector<int>& p)
                       {return isIntersected(path, p);}),
                i_balls[i].paths.end());
    }

    std::sort(i_balls.begin(), i_balls.end(),
              [](const Ball& b1, const Ball& b2)
              {return b1.paths.size() < b2.paths.size();});

    Ball ball = i_balls[0]; // ball with minimum count of paths

    if (ball.paths.size() == 0) {
        std::cerr << "error: no path fot this ball" << std::endl;
        return false;
    }

    for (size_t i = 0; i < ball.paths.size(); i++) {
         // call 'layPaths' for each path
        if (layPaths(i_balls, i_layed_paths, i)) {
            return true;
        }
    }
    return false;
}


int main()
{
    // 1. initialization
#ifdef WEB
    std::istream is(std::cin.rdbuf());
#endif
#ifndef WEB
    std::ifstream in("../3_winamax_sponsored_contest/test4.txt");
    if (!in.is_open()) {
        std::cerr << "error opening file" << std::endl;
        return -1;
    }
    // is = in;
    std::istream is(in.rdbuf());
#endif
    is >> width >> height; is.ignore();
    field = new char[width*height];
    std::vector<Ball> balls;
    for (int i = 0; i < height; i++) {
        std::string row;
        is >> row; is.ignore();
        for (int j = 0; j < width; j++) {
            char f = row[j];
            field[i*width+j] = f;
            if ((f >= '1') && (f <= '9')) {
                balls.push_back(Ball(f-'0', j, i));
            }
        }
    }


    // 2. search for availible paths for each ball
    for (auto i = balls.begin(); i != balls.end(); i++) {
        paths.clear();
        for (int k = 0; k < width*height; k++) {
            if (field[k] == CHECKED) {
                field[k] = EMPTY;
            }
        }

        findHole(i->shot_count, i->x, i->y);

        if (paths.size() == 0) {
            std::cerr << "error: no path found for some ball" << std::endl;
            return -1;
        }
        i->paths = paths;
    }


    // 3. lay paths
    std::vector< std::vector<int> > layed_paths;

    std::sort(balls.begin(), balls.end(),
              [](const Ball& b1, const Ball& b2)
              {return b1.paths.size() < b2.paths.size();});

    Ball ball = balls[0]; // ball with minimum count of paths

    if (ball.paths.size() == 0) {
        std::cerr << "error: no path fot this ball" << std::endl;
        return -1;
    }
    for (size_t i = 0; i < ball.paths.size(); i++) {
        if (layPaths(balls, layed_paths, i))
            break;
    }


    // 4. draw layed paths
    layed_paths = global_layed_paths;
    for (auto it = layed_paths.begin(); it != layed_paths.end(); it++) {
        std::vector<int> path = *it;
        field[path[0]] = USED_HOLE;
        for (size_t i = 0; i < path.size()-1; i++) {
            // next coords
            int xn = path[i] % width;
            int yn = path[i] / width;
            // previous coords
            int xp = path[i+1] % width;
            int yp = path[i+1] / width;
            // draw arrows
            // right
            if (xn > xp) for (int x = xp; x < xn; x++) field[yp*width+x] = '>';
            // left
            if (xn < xp) for (int x = xp; x > xn; x--) field[yp*width+x] = '<';
            // bottom
            if (yn > yp) for (int y = yp; y < yn; y++) field[y*width+xp] = 'v';
            // up
            if (yn < yp) for (int y = yp; y > yn; y--) field[y*width+xp] = '^';
        }
    }


    // 5. clean from special symbols
    for (int k = 0; k < width*height; k++) {
        if (   (field[k] == HOLE)  || (field[k] == CHECKED)
            || (field[k] == WATER) || (field[k] == USED_HOLE)) {
            field[k] = EMPTY;
        }
    }


    for (int ii = 0; ii < height; ii++) {
        for (int jj = 0; jj < width; jj++) {
            std::cout << field[ii*width+jj];
        }
        std::cout << std::endl;
    }

    return 0;
}
