#include <string.h>
#include <stdbool.h>
#include "types.h"

extern struct NODE* root;
extern struct NODE* cwd;

//make directory
void mkdir(char pathName[]) {
    
    char pathCopy[128];
    strcpy(pathCopy, pathName);

    //check if pathName is empty
    if (pathName[0] == '\0' || (pathName[0] == '/' && pathName[1] == '\0')) {
        printf("MKDIR ERROR: no path provided\n");
        return;
    }

    //allocate space for the new directory node
    NODE* targetDir = (NODE*)malloc(sizeof(NODE));

    //find the target directory to create the new directory in
    //cwd is the parent of targetDir
    //set targetDir's name to the base name of pathName
    char dirName[128]="";
    NODE* parent = splitPath(pathName, targetDir->name, dirName);
    if(!parent) {
        //error in splitPath, directory doesn't exist
        return;
    }

    //search tree to see if targetDir exists already
    //MKDIR ERROR: directory <DIRECTORY> already exists
    NODE* duplicate = parent->childPtr;
    while(duplicate) {
        if(strcmp(duplicate->name, targetDir->name) == 0) {
            printf("MKDIR ERROR: directory %s already exists\n", pathCopy);
            free(targetDir); // free allocated memory
            return;
        }
        duplicate = duplicate->siblingPtr; // move to next sibling
    }    
    
    targetDir->fileType = 'D';
    targetDir->parentPtr = parent; // set parent pointer to current working directory
    targetDir->childPtr = NULL; // initialize child pointer to NULL
    targetDir->siblingPtr = NULL; // initialize sibling pointer to NULL
    
    NODE* lastSibling = parent->childPtr;
    if(lastSibling == NULL) {
        parent->childPtr = targetDir; //set as first child if no children exist
    } else {
        
        while(lastSibling->siblingPtr != NULL) {
            lastSibling = lastSibling->siblingPtr; // find the last sibling
        }
        lastSibling->siblingPtr = targetDir; // set as the last sibling
    }

    printf("MKDIR SUCCESS: node %s successfully created\n", pathCopy);
    return;
}



/**
 * @brief The goal of the splitPath function is to split the path string into two parts: dirName and baseName, 
 * which are character arrays setup by the caller function. The dirName is a sequence of directory names that show the path to the baseName, 
 * which is the name of the target file or directory. Should return a pointer to the node that represents the parent directory of the baseName.
 * @param pathName 
 * @param baseName 
 * @param dirName 
 * @return struct NODE* 
 */
//handles tokenizing and absolute/relative pathing options
struct NODE* splitPath(char* pathName, char* baseName, char* dirName){

    bool isAbsolute = (pathName[0] == '/');

    char pathCopy[64] = {0};
    strcpy(pathCopy, pathName);

    //set parentDirectory to root for absolute paths, cwd for relative paths
    NODE* parentDirectory = isAbsolute ? root : cwd;
    char* token = strtok(pathCopy, "/");
    dirName[0] = '\0';

    int count = 0;
    char* tokens[32];
    while (token && count < 32) {
        tokens[count++] = token;
        token = strtok(NULL, "/");
    }

    //if no tokens are found, return root
    if(count == 0) {
        dirName[0] = '/';
        dirName[1] = '\0';
        baseName[0] = '\0';
        return root; //return root
    }
    
    //if only one token is found, set baseName to that token
    strcpy(baseName, tokens[count - 1]); 

    //build dirName and check if every directory in the path exists
    for (int i = 0; i < count - 1; i++) {
        struct NODE* child = findChild(parentDirectory, tokens[i]);
        
        if (!child) {
            printf("ERROR: directory %s does not exist\n", tokens[i]);
            dirName[0] = '\0';
            baseName[0] = '\0';
            return NULL;
        }

        //update parent directory
        parentDirectory = child;

        //Begin building dirName
        if (i == 0 && isAbsolute) {
            // we started with '/', so dirName starts with '/'
            strcpy(dirName, "/");
            strcat(dirName, tokens[i]);
        } else {
            if (i>0 || !isAbsolute) {
                if (dirName[0] != '\0') strcat(dirName, "/");
            }
            strcat(dirName, tokens[i]);
        }
    }

    return parentDirectory;
}

/**
 * @brief The goal of the searchTree function is to search the tree for a directory with the name dirName
 * and return it if found. If not found, return NULL. Uses level order traversal to search the tree.
 * @param dirName 
 */
struct NODE* searchTree(struct NODE* start, char* dirName) {
    NODE* currentPtr = start;
    if(strcmp(dirName, currentPtr->name) == 0) {
        return currentPtr;
    }

    //Traverse the tree using level order traversal.
    //While the current pointer has children or siblings, check if the name matches dirName
    //If it does, return the current pointer
    //If not, move to the next sibling or child
    //If no match is found, return NULL
    while(currentPtr->childPtr != NULL || currentPtr->siblingPtr != NULL) {
        if(currentPtr->siblingPtr == NULL) {
            currentPtr = currentPtr->childPtr;
        } else {
            currentPtr = currentPtr->siblingPtr;
        }
        if(strcmp(dirName, currentPtr->name) == 0) {
            return currentPtr;
        }
    }
    return NULL;
}

NODE* findChild(NODE* parent, char* name) {
    //If parent is null or has no children, return NULL right away
    if (!parent || !parent->childPtr) {
        return NULL;
    }

    //Start from the first child of parent and iterate through the siblings
    NODE* current=parent->childPtr;
    while(current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current=current->siblingPtr;
    }
    return NULL;
}