// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>
#include <layoff.h>

char somestringBuf[200];

// Main program entrypoint
int main(int argc, char* argv[])
{
    // This example uses a text console, as a simple way to output text to the screen.
    // If you want to write a software-rendered graphics application,
    //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
    // If on the other hand you want to write an OpenGL based application,
    //   take a look at the graphics/opengl set of examples, which uses EGL instead.
    consoleInit(NULL);

    // Other initialization goes here. As a demonstration, we print hello world.
	printf("Press A to send test notification\n");
	printf("Press B to send a text control\n");
	printf("Press Y to send a button list\n");
	
	printf("Press X to remove all controls\n");
	printf("Press L to to wait for a remote input\n");
	printf("Press R to to check for a remote input\n");
    
	Result rc = layoffInitialize("lib-test");
    printf("layoffInitialize(): %x\n", rc);

	srand(time(NULL));

    // Main loop
    while (appletMainLoop())
    {
        // Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // hidKeysDown returns information about which buttons have been
        // just pressed in this frame compared to the previous one
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; // break in order to return to hbmenu
		else if (kDown & KEY_A)
		{
			snprintf(somestringBuf, sizeof(somestringBuf), "Hello world! %d", rand() % 100);
			printf("Sent a notification, response: %x\n", layoffNotifySimple(somestringBuf));
		}
		else if (kDown & KEY_B)
		{
			snprintf(somestringBuf, sizeof(somestringBuf),
				"Lorem ipsum dolor sit amet %d, consectetur adipiscing elit.%d Vivamus non metus sit amet %d elit blandit viverra vel a volutpat. %d",
				rand() % 100, rand() % 100, rand() % 100, rand() % 100);

			LayoffUIHeader header;
			header.kind = LayoffUIKind_TextBlock;
			header.panelID = 1;
			header.encoding = LayoffEncoding_ASCII;

			printf("response: %x\n", layoffPushUIPanel(header, somestringBuf, strlen(somestringBuf) + 1));
		}
		else if (kDown & KEY_Y)
		{
			LayoffUIHeader header;
			header.kind = LayoffUIKind_ButtonList;
			header.panelID = 2;
			header.encoding = LayoffEncoding_ASCII;

			const u32 sz = sizeof(LayoffUIButtonList) + sizeof(LayoffName) * 3;
			LayoffUIButtonList* buttons = malloc(sz);
			buttons->ButtonCount = 3;
			strcpy(buttons->data[0].str, "first");
			strcpy(buttons->data[1].str, "second");
			strcpy(buttons->data[2].str, "third");

			printf("response: %x\n", layoffPushUIPanel(header, buttons, sz));
			
			free(buttons);
		}
		else if (kDown & KEY_L)
		{
			LayoffUIEvent event;
			do {
				layoffGetLastUIEvent(&event);
				svcSleepThread(3E+8);
			} while (!event.panel);
			printf("Last event is from pane : %d\n", event.panel);
			printf("Button %d was pressed\n", event.data1 + 1);
		}
		else if (kDown & KEY_R)
		{
			LayoffUIEvent event;
			printf("response: %x\n", layoffGetLastUIEvent(&event));
			printf("Last event is from pane : %d\n", event.panel);
			if (event.panel)
				printf("Button %d was pressed\n", event.data1 + 1);
		}
		else if (kDown & KEY_X)
		{
			LayoffUIHeader header;
			header.kind = LayoffUIKind_None;
			header.panelID = 1;

			printf("response: %x\n", layoffPushUIPanel(header, NULL, 0));

			header.panelID = 2;
			printf("response: %x\n", layoffPushUIPanel(header, NULL, 0));
		}

        // Your code goes here

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    layoffExit();

    // Deinitialize and clean up resources used by the console (important!)
    consoleExit(NULL);
    return 0;
}
