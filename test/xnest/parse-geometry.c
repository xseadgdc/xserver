
#include <stdio.h>
#include <xcb/xcb.h>

#include "../../hw/xnest/parse-geometry.h"

#define ARRAY_SIZE(a)  (sizeof((a)) / sizeof((a)[0]))

struct testcase {
    const char *s;
    xRectangle r;
    int m;
};

struct testcase testcases[] = {
    {
        .s = "+27+148",
        .r = { .x = 27, .y = 148 },
        .m = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
    },
    {
        .s = "+0-196",
        .r = { .x = 0, .y = -196 },
        .m = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
    },
    {
        .s = "=+0-196",
        .r = { .x = 0, .y = -196 },
        .m = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
    },
    {
        .s = "150x600-10+10",
        .r = { .x = -10, .y = 10, .width = 150, .height = 600 },
        .m = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
    },
    {
        .s = "=150x600-10+10",
        .r = { .x = -10, .y = 10, .width = 150, .height = 600 },
        .m = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
    },
    {
        .s = "=150x600",
        .r = { .width = 150, .height = 600 },
        .m = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
    },
};

int main(int argc, char **argv)
{
    int status = 0;
    for (int x=0; x<ARRAY_SIZE(testcases); x++) {
        xRectangle r = { 0 };
        printf("Test case %s\n", testcases[x].s);
        int mask = xnest_parse_geometry(testcases[x].s, &r);
        if (mask & XCB_CONFIG_WINDOW_X)
            printf("X: %d\n", r.x);
        if (mask & XCB_CONFIG_WINDOW_Y)
            printf("Y: %d\n", r.y);
        if (mask & XCB_CONFIG_WINDOW_WIDTH)
            printf("W: %d\n", r.width);
        if (mask & XCB_CONFIG_WINDOW_HEIGHT)
            printf("H: %d\n", r.height);

        if (testcases[x].r.x != r.x) {
            fprintf(stderr, "#%d \"%s\" X mismatch %d instead of %d\n",
                x, testcases[x].s, r.x, testcases[x].r.x);
            status++;
        }
        if (testcases[x].r.y != r.y) {
            fprintf(stderr, "#%d \"%s\" Y mismatch %d instead of %d\n",
                x, testcases[x].s, r.y, testcases[x].r.y);
            status++;
        }
        if (testcases[x].r.width != r.width) {
            fprintf(stderr, "#%d \"%s\" W mismatch %d instead of %d\n",
                x, testcases[x].s, r.width, testcases[x].r.width);
            status++;
        }
        if (testcases[x].r.height!= r.height) {
            fprintf(stderr, "#%d \"%s\" W mismatch %d instead of %d\n",
                x, testcases[x].s, r.height, testcases[x].r.height);
            status++;
        }
        if (testcases[x].m != mask) {
            fprintf(stderr, "#%d \"%s\" M mismatch %d instead of %d\n",
                x, testcases[x].s, mask, testcases[x].m);
            status++;
        }
    }

    if (status)
        fprintf(stderr, "%d items failed\n", status);
    else
        printf("OK\n");

    return status;
}
