#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <layoff.h>
#include <layoffUI.h>

char somestringBuf[200];

int main(int argc, char* argv[])
{
    consoleInit(NULL);

	printf(
		"liblayoff example:\n\n"
		"Notifications:\n"
		"Press A to send test notification\n"
		"\nControls:\n"
		"Press B to send a text block\n"
		"Press Y to send a button list\n"
		"Press DPAD_LEFT to send a checkbox list\n"
		"Press DPAD_UP to send a radiobutton list\n"
		"Press DPAD_RIGHT to send a combobox\n"
		"Press X to remove all controls\n"
		"\nEvents\n:"
		"Press L to to wait for a remote input\n"
		"Press R to to check for a remote input\n"
		"Press - to continuously wait for remote input\n"
		"\nPress start to quit.\n"
	);
    
	Result rc = layoffInitialize("lib-test");
    printf("layoffInitialize(): %x\n", rc);

	srand(time(NULL));

	//This is not necessary, if you're not going to read it don't acquire it
	Event evt;
	rc = layoffAcquireUIEvent(&evt);
	printf("acquireUIEvent(): %x\n", rc);

    while (appletMainLoop())
    {
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS)
            break; //Quit on purpose without disposing panes to test client disconnect detection
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
			//Panels are sorted based on their panelID, 0 is not a valid value
			layoffInitializeUIHeader(&header, 1, LayoffUIKind_TextBlock);

			//For LayoffUIKind_TextBlock the payload is just the ASCII null-terminated string
			printf("response: %x\n", layoffPushUIPanel(header, somestringBuf, strlen(somestringBuf) + 1));
		}
		else if (kDown & KEY_Y)
		{
			LayoffUIHeader header;
			LayoffUIButtonList* payload = NULL;

			layoffInitializeUIHeader(&header, 2, LayoffUIKind_TextBlock);
			printf("response: %x\n", layoffPushUIPanel(header, "Some text next to a button", strlen("Some text next to a button") + 1));

			header = (LayoffUIHeader){0};

			//Note that internally imgui uses the label to identify the control, this means that having two labels that have the same value in different panels will break.
			//TODO: work on a fix, currently workaround adding #1 according to imgui docs
			u32 payloadSize = layoffNewButtonList(&header, 3, &payload, 9, "first", "second", "third", "fourth", "fith", "sixth", "more", "they never end", "button");
			if (!payloadSize)
			{
				printf("Error !");
			}
			else
			{
				layoffSetInlineFlag(payload, 0, true); // first comes after the text pane
				
				layoffSetInlineFlag(payload, 2, true); // Third is in the same line as second
				
				layoffSetInlineFlag(payload, 4, true); // Fifth and sixth are in the same line as third
				layoffSetInlineFlag(payload, 5, true); 
				
				printf("response: %x\n", layoffPushUIPanel(header, payload, payloadSize));
				layoffFreeButtonList(payload);
			}

			//For performance reasons it's possible to manually generate or modify the payload:
				//const u32 sz = sizeof(LayoffUIButtonList) + sizeof(LayoffName) * 3;
				//LayoffUIButtonList* buttons = malloc(sz);
				//buttons->count = 3;
				//strcpy(buttons->data[0].str, "first");
				//strcpy(buttons->data[1].str, "second");
				//strcpy(buttons->data[2].str, "third");
		}
		else if (kDown & KEY_DUP)
		{
			LayoffUIHeader header;
			LayoffUICheckBoxList* payload;

			u32 payloadSize = layoffNewRadioButtonList(&header, 4, &payload, 4, "BUG #23", "BUG #34", "BUG #14", "BUG #12");
			if (!payloadSize)
			{
				printf("Error !");
			}
			else
			{
				layoffSetInlineFlag(payload, 1, true);
				layoffSetInlineFlag(payload, 3, true);
				printf("response: %x\n", layoffPushUIPanel(header, payload, payloadSize));
				layoffFreeRadioButtonList(payload);
			}
		}
		else if (kDown & KEY_DLEFT)
		{
			LayoffUIHeader header;
			LayoffUICheckBoxList* payload;

			u32 payloadSize = layoffNewCheckboxList(&header, 5, &payload, 4, "first", "second", "third", "fourth");
			if (!payloadSize)
			{
				printf("Error !");
			}
			else
			{
				layoffSetInlineFlag(payload, 1, true);
				layoffSetInlineFlag(payload, 3, true);
				printf("response: %x\n", layoffPushUIPanel(header, payload, payloadSize));
				layoffFreeCheckBoxList(payload);
			}
		}
		else if (kDown & KEY_DRIGHT)
		{
			LayoffUIHeader header;
			LayoffUIComboBox* payload;

			u32 payloadSize = layoffNewComboBox(&header, 6, &payload, "ComboBox test", 4, "first", "second", "third", "fourth");
			if (!payloadSize)
			{
				printf("Error !");
			}
			else
			{
				printf("response: %x\n", layoffPushUIPanel(header, payload, payloadSize));
				layoffFreeComboBox(payload);
			}
		}
		else if (kDown & KEY_L)
		{
			eventWait(&evt, UINT64_MAX);
			LayoffUIEvent event;
			printf("response: %x\n", layoffGetLastUIEvent(&event));
			printf("Last event is from pane : %d\n", event.panel);
			if (event.panel)
				printf("Event data is %lx %lx\n", event.data1, event.data2);
		}
		else if (kDown & KEY_R)
		{
			LayoffUIEvent event;
			printf("response: %x\n", layoffGetLastUIEvent(&event));
			printf("Last event is from pane : %d\n", event.panel);
			if (event.panel)
				printf("Event data is %lx %lx\n", event.data1, event.data2);
		}
		else if (kDown & KEY_X)
		{			
			//Doesn't matter if one of these doesn't currently exist
			printf("response: %x\n", layoffRemoveUIPanel(1));
			printf("response: %x\n", layoffRemoveUIPanel(2));
			printf("response: %x\n", layoffRemoveUIPanel(3));
			printf("response: %x\n", layoffRemoveUIPanel(4));
			printf("response: %x\n", layoffRemoveUIPanel(5));
			printf("response: %x\n", layoffRemoveUIPanel(6));
		}
		else if (kDown & KEY_MINUS)
		{
			LayoffUIHeader header;
			LayoffUIButtonList* payload = NULL;

			u32 payloadSize = layoffNewButtonList(&header, 10, &payload, 1, "STOP");
			if (!payloadSize)
			{
				printf("Error !");
			}
			else
			{
				printf("Push STOP button: %x\n", layoffPushUIPanel(header, payload, payloadSize));
				layoffFreeButtonList(payload);

				printf("Press the STOP button in layoff to resume this application.\n");

				consoleUpdate(NULL);

				while (true)
				{
					eventWait(&evt, UINT64_MAX);
					LayoffUIEvent event;
					printf("response: %x\n", layoffGetLastUIEvent(&event));
					
					if (event.panel == 10) break;
					
					printf("Last event is from pane : %d\n", event.panel);
					if (event.panel)
						printf("Event data is %lx %lx\n", event.data1, event.data2);
					consoleUpdate(NULL);
				}
			}
		}

        consoleUpdate(NULL);
    }

    layoffExit();

    consoleExit(NULL);
    return 0;
}
