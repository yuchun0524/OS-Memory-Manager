#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Node
{
    int pfi;
    int in_use;
    int present;
};
typedef struct Node node;
struct Memory
{
    int frame;
    int order;
};
typedef struct Memory memory;
struct Second
{
    int vpi;
    int reference;
    int index;
    int dirty;
    struct Second *next;
};
typedef struct Second second;
struct LRU
{
    int vpi;
    int reference;
    struct LRU *pre;
    struct LRU *next;
};
typedef struct LRU lru;
int main()
{
    char str[60];
    char miss[100];
    fgets(str, 60, stdin);
    char *policy_pointer, *page, *frame_num;
    char policy[10];
    int page_size, frame_size;
    int count = 0, count_miss = 0, disk_free = 0, source = 0;
    bool memory_full;
    policy_pointer = strchr(str, ':') + 2;
    sprintf(policy, policy_pointer);
    fgets(str, 60, stdin);
    page = strchr(str, ':') + 2;
    page_size = atoi(page);
    int disk[page_size];
    node *virtual = (node *)malloc(page_size * sizeof(node));
    for (int i = 0; i < page_size; i++)
    {
        disk[i] = -1;
    }
    for (int i = 0; i < page_size; i++)
    {
        virtual[i].in_use = 0;
        virtual[i].pfi = 0;
        virtual[i].present = 0;
    }
    fgets(str, 60, stdin);
    frame_num = strchr(str, ':') + 2;
    frame_size = atoi(frame_num);
    if (strcmp(policy, "FIFO\n") == 0)
    {
        char *reference;
        int refer;
        int memory_order = 0;
        int compare_order = 99999;
        memory *physical = (memory *)malloc(frame_size * sizeof(memory));
        for (int i = 0; i < frame_size; i++)
        {
            physical[i].frame = -1;
            physical[i].order = 0;
        }
        fgets(str, 60, stdin);
        while (1)
        {
            if (fgets(str, 60, stdin) == NULL)
                break;
            else
            {
                count++;
                strtok_r(str, " ", &reference);
                refer = atoi(reference);
                if (virtual[refer].present == 1)
                    printf("Hit, %d=>%d\n", refer, virtual[refer].pfi);
                else
                {
                    if (memory_order < frame_size)
                        memory_order++;
                    memory_full = true;
                    for (int i = 0; i < frame_size; i++)
                    {
                        if (physical[i].frame == -1)
                        {
                            virtual[refer].pfi = i;
                            virtual[refer].present = 1;
                            memset(miss, 0, sizeof(miss));
                            char number[10];
                            sprintf(number, "%d", virtual[refer].pfi);
                            strcat(number, "\0");
                            strcat(miss, number);
                            strcat(miss, ", -1>>-1, ");
                            physical[i].frame = refer;
                            physical[i].order = memory_order;
                            memory_full = false;
                            break;
                        }
                    }
                    if (memory_full)
                    {
                        compare_order = 99999;
                        for (int i = 0; i < frame_size; i++)
                        {
                            compare_order = (compare_order > physical[i].order) ? physical[i].order : compare_order;
                        }
                        for (int i = 0; i < frame_size; i++)
                        {
                            if (compare_order == physical[i].order)
                            {
                                virtual[refer].present = 1;
                                virtual[refer].pfi = i;
                                for (int j = 0; j < page_size; j++)
                                {
                                    if (disk[j] == -1)
                                    {
                                        disk_free = j;
                                        break;
                                    }
                                }
                                for (int j = 0; j < frame_size; j++)
                                {
                                    if (physical[j].frame != -1 && physical[i].order != physical[j].order)
                                        physical[j].order--;
                                }
                                disk[disk_free] = physical[i].frame;
                                virtual[physical[i].frame].present = 0;
                                virtual[physical[i].frame].pfi = disk_free;
                                physical[i].order = memory_order;
                                char num[10];
                                memset(miss, 0, sizeof(miss));
                                sprintf(num, "%d", i);
                                strcat(num, "\0");
                                strcat(miss, num);
                                strcat(miss, ", ");
                                char evict[10];
                                sprintf(evict, "%d", physical[i].frame);
                                strcat(evict, "\0");
                                strcat(miss, evict);
                                strcat(miss, ">>");
                                char des[10];
                                sprintf(des, "%d", disk_free);
                                strcat(des, "\0");
                                strcat(miss, des);
                                strcat(miss, ", ");
                                physical[i].frame = refer;
                                break;
                            }
                        }
                    }
                    if (virtual[refer].in_use == 1)
                    {
                        source = -1;
                        for (int i = 0; i < page_size; i++)
                        {
                            if (disk[i] == refer)
                            {
                                source = i;
                                disk[i] = -1;
                                break;
                            }
                        }
                        printf("Miss, %s%d<<%d\n", miss, refer, source);
                    }
                    else
                    {
                        printf("Miss, %s%d<<-1\n", miss, refer);
                    }
                    count_miss++;
                }
                virtual[refer].in_use = 1;
            }
        }
        printf("Page Fault Rate: %.3f", (float)count_miss / count);
    }
    else if (strcmp(policy, "ESCA\n") == 0)
    {
        char *input, *copy;
        char option[10];
        int input_vpi;
        second *head, *current, *tail, *pointer;
        for (int i = 0; i < frame_size; i++)
        {
            current = (second *)malloc(sizeof(second));
            current->dirty = 0;
            current->reference = 0;
            current->index = i;
            current->vpi = -1;
            if (i == 0)
            {
                head = current;
                tail = current;
                pointer = head;
            }
            else
            {
                tail->next = current;
                tail = current;
            }
            current->next = NULL;
            if (i == frame_size - 1)
            {
                tail->next = head;
            }
        }
        fgets(str, 60, stdin);
        while (1)
        {
            if (fgets(str, 60, stdin) == NULL)
                break;
            else
            {
                count++;
                copy = str;
                strtok_r(str, " ", &input);
                input_vpi = atoi(input);
                if (virtual[input_vpi].present == 1)
                {
                    printf("Hit, %d=>%d\n", input_vpi, virtual[input_vpi].pfi);
                    current = head;
                    for (int i = 0; i < frame_size; i++)
                    {
                        if (current->vpi == input_vpi)
                        {
                            current->reference = 1;
                            if (strcmp(str, "Write") == 0)
                            {
                                current->dirty = 1;
                            }
                            break;
                        }
                        current = current->next;
                    }
                }
                else
                {
                    memory_full = true;
                    current = head;
                    for (int i = 0; i < frame_size; i++)
                    {
                        if (current->vpi == -1)
                        {
                            virtual[input_vpi].pfi = current->index;
                            virtual[input_vpi].present = 1;
                            memset(miss, 0, sizeof(miss));
                            char number[10];
                            sprintf(number, "%d", virtual[input_vpi].pfi);
                            strcat(number, "\0");
                            strcat(miss, number);
                            strcat(miss, ", -1>>-1, ");
                            current->vpi = input_vpi;
                            current->reference = 1;
                            current->dirty = 0;
                            if (strcmp(str, "Write") == 0)
                            {
                                current->dirty = 1;
                            }
                            memory_full = false;
                            break;
                        }
                        current = current->next;
                    }
                    if (memory_full)
                    {
                        bool is_break = false;
                        while (1)
                        {
                            current = pointer;
                            for (int i = 0; i < frame_size; i++)
                            {
                                if (current->reference == 0 && current->dirty == 0)
                                {
                                    is_break = true;
                                    for (int j = 0; j < page_size; j++)
                                    {
                                        if (disk[j] == -1)
                                        {
                                            disk_free = j;
                                            break;
                                        }
                                    }
                                    char num[10];
                                    memset(miss, 0, sizeof(miss));
                                    sprintf(num, "%d", current->index);
                                    strcat(num, "\0");
                                    strcat(miss, num);
                                    strcat(miss, ", ");
                                    char evict[10];
                                    sprintf(evict, "%d", current->vpi);
                                    strcat(evict, "\0");
                                    strcat(miss, evict);
                                    strcat(miss, ">>");
                                    char des[10];
                                    sprintf(des, "%d", disk_free);
                                    strcat(des, "\0");
                                    strcat(miss, des);
                                    strcat(miss, ", ");
                                    disk[disk_free] = current->vpi;
                                    virtual[current->vpi].present = 0;
                                    virtual[current->vpi].pfi = disk_free;
                                    current->vpi = input_vpi;
                                    current->dirty = 0;
                                    current->reference = 1;
                                    if (strcmp(str, "Write") == 0)
                                    {
                                        current->dirty = 1;
                                    }
                                    virtual[input_vpi].pfi = current->index;
                                    virtual[input_vpi].present = 1;
                                    break;
                                }
                                current = current->next;
                                pointer = current;
                            }
                            if (is_break)
                            {
                                pointer = pointer->next;
                                break;
                            }
                            current = pointer;
                            for (int i = 0; i < frame_size; i++)
                            {
                                if (current->reference == 0 && current->dirty == 1)
                                {
                                    is_break = true;
                                    for (int j = 0; j < page_size; j++)
                                    {
                                        if (disk[j] == -1)
                                        {
                                            disk_free = j;
                                            break;
                                        }
                                    }
                                    char num[10];
                                    memset(miss, 0, sizeof(miss));
                                    sprintf(num, "%d", current->index);
                                    strcat(num, "\0");
                                    strcat(miss, num);
                                    strcat(miss, ", ");
                                    char evict[10];
                                    sprintf(evict, "%d", current->vpi);
                                    strcat(evict, "\0");
                                    strcat(miss, evict);
                                    strcat(miss, ">>");
                                    char des[10];
                                    sprintf(des, "%d", disk_free);
                                    strcat(des, "\0");
                                    strcat(miss, des);
                                    strcat(miss, ", ");
                                    disk[disk_free] = current->vpi;
                                    virtual[current->vpi].present = 0;
                                    virtual[current->vpi].pfi = disk_free;
                                    current->vpi = input_vpi;
                                    current->dirty = 0;
                                    current->reference = 1;
                                    if (strcmp(str, "Write") == 0)
                                    {
                                        current->dirty = 1;
                                    }
                                    virtual[input_vpi].pfi = current->index;
                                    virtual[input_vpi].present = 1;
                                    break;
                                }
                                current->reference = 0;
                                current = current->next;
                                pointer = current;
                            }
                            if (is_break)
                            {
                                pointer = pointer->next;
                                break;
                            }
                        }
                    }
                    if (virtual[input_vpi].in_use == 1)
                    {
                        source = -1;
                        for (int i = 0; i < page_size; i++)
                        {
                            if (disk[i] == input_vpi)
                            {
                                source = i;
                                disk[i] = -1;
                                break;
                            }
                        }
                        printf("Miss, %s%d<<%d\n", miss, input_vpi, source);
                    }
                    else
                    {
                        printf("Miss, %s%d<<-1\n", miss, input_vpi);
                    }
                    count_miss++;
                }
                virtual[input_vpi].in_use = 1;
            }
        }
        printf("Page Fault Rate: %.3f", (float)count_miss / count);
    }
    else if (strcmp(policy, "SLRU\n") == 0)
    {
        char *input;
        int input_vpi, count_active = 0, count_inactive = 0, index;
        double active = 0, inactive = 0;
        bool is_active, free_active, free_inactive;
        lru *head_active = NULL, *current = NULL, *tail_active = NULL, *temp = NULL, *head = NULL, *tail = NULL;
        active = frame_size / 2;
        inactive = frame_size - active;
        int *physical = malloc(frame_size * sizeof(int));
        for (int i = 0; i < frame_size; i++)
        {
            physical[i] = -1;
        }
        fgets(str, 60, stdin);
        while (1)
        {
            if (fgets(str, 60, stdin) == NULL)
                break;
            else
            {
                count++;
                strtok_r(str, " ", &input);
                input_vpi = atoi(input);
                if (virtual[input_vpi].present == 1)
                {
                    printf("Hit, %d=>%d\n", input_vpi, virtual[input_vpi].pfi);
                    if (active <= 0)
                        is_active = false;
                    else
                    {
                        current = head_active;
                        is_active = false;
                        while (current != NULL)
                        {
                            if (current->vpi == input_vpi)
                            {
                                is_active = true;
                                break;
                            }
                            current = current->next;
                        }
                    }
                    if (is_active)
                    {
                        if (count_active > 1)
                        {
                            if (current != head_active)
                            {
                                if (current == tail_active)
                                {
                                    tail_active = tail_active->pre;
                                    tail_active->next = NULL;
                                }
                                else
                                {
                                    current->pre->next = current->next;
                                    current->next->pre = current->pre;
                                }
                                current->next = head_active;
                                head_active->pre = current;
                                head_active = current;
                                current->pre = NULL;
                            }
                        }
                        if (current->reference == 0)
                            current->reference = 1;
                    }
                    else
                    {
                        current = head;
                        while (current != NULL)
                        {
                            if (current->vpi == input_vpi)
                                break;
                            current = current->next;
                        }
                        if (current != NULL)
                        {
                            if (active > 0)
                            {
                                if (current->reference == 0)
                                {
                                    current->reference = 1;
                                    if (current != head)
                                    {
                                        if (current == tail)
                                        {
                                            tail = tail->pre;
                                            tail->next = NULL;
                                        }
                                        else
                                        {
                                            current->pre->next = current->next;
                                            current->next->pre = current->pre;
                                        }
                                        current->next = head;
                                        head->pre = current;
                                        current->pre = NULL;
                                        head = current;
                                    }
                                }
                                else
                                {
                                    if (count_active < active)
                                        free_active = true;
                                    else
                                        free_active = false;
                                    current->reference = 0;
                                    if (count_active >= 1)
                                    {
                                        if (free_active)
                                        {
                                            if (current == head)
                                            {
                                                head = head->next;
                                            }
                                            else if (current == tail)
                                            {
                                                tail = tail->pre;
                                                tail->next = NULL;
                                            }
                                            else
                                            {
                                                current->pre->next = current->next;
                                                current->next->pre = current->pre;
                                            }
                                            current->next = head_active;
                                            head_active->pre = current;
                                            current->pre = NULL;
                                            head_active = current;
                                            count_active++;
                                            count_inactive--;
                                        }
                                        else
                                        {
                                            while (1)
                                            {
                                                temp = tail_active;
                                                if (temp->reference == 0)
                                                {
                                                    tail_active = tail_active->pre;
                                                    tail_active->next = NULL;
                                                    temp->next = head;
                                                    head->pre = temp;
                                                    temp->pre = NULL;
                                                    head = temp;
                                                    break;
                                                }
                                                else
                                                {
                                                    temp->reference = 0;
                                                    tail_active = tail_active->pre;
                                                    tail_active->next = NULL;
                                                    temp->next = head_active;
                                                    head_active->pre = temp;
                                                    temp->pre = NULL;
                                                    head_active = temp;
                                                }
                                            }
                                            if (current == tail)
                                            {
                                                tail = tail->pre;
                                                tail->next = NULL;
                                            }
                                            else
                                            {
                                                current->pre->next = current->next;
                                                current->next->pre = current->pre;
                                            }
                                            current->next = head_active;
                                            head_active->pre = current;
                                            current->pre = NULL;
                                            head_active = current;
                                        }
                                    }
                                    else if (count_active == 0)
                                    {
                                        head_active = current;
                                        tail_active = current;
                                        if (current == head)
                                        {
                                            head = head->next;
                                        }
                                        else if (current == tail)
                                        {
                                            tail = tail->pre;
                                            tail->next = NULL;
                                        }
                                        else
                                        {
                                            current->pre->next = current->next;
                                            current->next->pre = current->pre;
                                        }
                                        current->next = NULL;
                                        current->pre = NULL;
                                        count_active++;
                                        count_inactive--;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (count_inactive < inactive)
                        free_inactive = true;
                    else
                        free_inactive = false;
                    virtual[input_vpi].present = 1;
                    if (free_inactive)
                    {
                        for (int i = 0; i < frame_size; i++)
                        {
                            if (physical[i] == -1)
                            {
                                physical[i] = input_vpi;
                                index = i;
                                break;
                            }
                        }
                        if (count_inactive == 0)
                        {
                            current = (lru *)malloc(sizeof(lru));
                            count_inactive++;
                            head = current;
                            tail = current;
                            current->reference = 1;
                            current->vpi = input_vpi;
                            current->next = NULL;
                            current->pre = NULL;
                        }
                        else if (count_inactive >= 1)
                        {
                            count_inactive++;
                            current = (lru *)malloc(sizeof(lru));
                            current->next = head;
                            head->pre = current;
                            head = current;
                            current->reference = 1;
                            current->vpi = input_vpi;
                            current->pre = NULL;
                        }
                        virtual[input_vpi].pfi = index;
                        physical[index] = input_vpi;
                        char num[10];
                        memset(miss, 0, sizeof(miss));
                        sprintf(num, "%d", index);
                        strcat(num, "\0");
                        strcat(miss, num);
                        strcat(miss, ", ");
                        strcat(miss, "-1>>-1, ");
                    }
                    else
                    {
                        if (inactive == 1)
                        {
                            for (int i = 0; i < page_size; i++)
                            {
                                if (disk[i] == -1)
                                {
                                    disk_free = i;
                                    disk[i] = head->vpi;
                                    break;
                                }
                            }
                            memset(miss, 0, sizeof(miss));
                            strcat(miss, "0");
                            strcat(miss, ", ");
                            char evict[10];
                            sprintf(evict, "%d", head->vpi);
                            strcat(evict, "\0");
                            strcat(miss, evict);
                            strcat(miss, ">>");
                            char des[10];
                            sprintf(des, "%d", disk_free);
                            strcat(des, "\0");
                            strcat(miss, des);
                            strcat(miss, ", ");
                            virtual[input_vpi].pfi = 0;
                            virtual[physical[0]].present = 0;
                            virtual[physical[0]].pfi = disk_free;
                            head->reference = 1;
                            head->vpi = input_vpi;
                            physical[0] = input_vpi;
                        }
                        else
                        {
                            while (1)
                            {
                                temp = tail;
                                if (temp->reference == 0)
                                {
                                    for (int i = 0; i < page_size; i++)
                                    {
                                        if (disk[i] == -1)
                                        {
                                            disk_free = i;
                                            disk[i] = temp->vpi;
                                            break;
                                        }
                                    }
                                    for (int i = 0; i < frame_size; i++)
                                    {
                                        if (physical[i] == temp->vpi)
                                        {
                                            index = i;
                                            physical[i] = input_vpi;
                                            break;
                                        }
                                    }
                                    virtual[temp->vpi].pfi = disk_free;
                                    virtual[temp->vpi].present = 0;
                                    virtual[input_vpi].pfi = index;
                                    char num[10];
                                    memset(miss, 0, sizeof(miss));
                                    sprintf(num, "%d", index);
                                    strcat(num, "\0");
                                    strcat(miss, num);
                                    strcat(miss, ", ");
                                    char evict[10];
                                    sprintf(evict, "%d", temp->vpi);
                                    strcat(evict, "\0");
                                    strcat(miss, evict);
                                    strcat(miss, ">>");
                                    char des[10];
                                    sprintf(des, "%d", disk_free);
                                    strcat(des, "\0");
                                    strcat(miss, des);
                                    strcat(miss, ", ");
                                    physical[index] = input_vpi;
                                    tail = tail->pre;
                                    tail->next = NULL;
                                    free(temp);
                                    temp = NULL;
                                    break;
                                }
                                else
                                {
                                    temp->reference = 0;
                                    tail = tail->pre;
                                    tail->next = NULL;
                                    temp->next = head;
                                    head->pre = temp;
                                    temp->pre = NULL;
                                    head = temp;
                                }
                            }
                            current = (lru *)malloc(sizeof(lru));
                            current->next = head;
                            head->pre = current;
                            head = current;
                            current->reference = 1;
                            current->vpi = input_vpi;
                            current->pre = NULL;
                        }
                    }
                    if (virtual[input_vpi].in_use == 1)
                    {
                        source = -1;
                        for (int i = 0; i < page_size; i++)
                        {
                            if (disk[i] == input_vpi)
                            {
                                source = i;
                                disk[i] = -1;
                                break;
                            }
                        }
                        printf("Miss, %s%d<<%d\n", miss, input_vpi, source);
                    }
                    else
                    {
                        printf("Miss, %s%d<<-1\n", miss, input_vpi);
                    }
                    count_miss++;
                }
                virtual[input_vpi].in_use = 1;
            }
        }
        printf("Page Fault Rate: %.3f", (float)count_miss / count);
    }
}