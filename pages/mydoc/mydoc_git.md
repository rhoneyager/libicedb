---
title: How to use git and contribute to libicedb
tags: [getting_started, troubleshooting, collaboration]
last_updated: May 23, 2018
keywords: git, jekyll, documentation
summary: "Step by step guide for libicedb developers"
sidebar: mydoc_sidebar
permalink: usegit.html
folder: mydoc
---

This is a step by step guide on how to setup git on your system and start contributing to the libicedb code library. Unfortunately it is not meant to be general. It addresses immediately Ubuntu Linux users, but it is easy to extend to other OS.

We do not want to dive too much into the concept and technicalities of git, for that, you might want to have a look at some more official [tutorial](https://services.github.com/on-demand/intro-to-github/)

You might find it worthwhile to learn how Git works. It is useful for any project where you want either version control or the ability to sync changes across multiple machines. It's perfect for both computer code and LaTeX documents.

## Setup a git account and install git client

If you are here, it is assumed you already have a github account. Another prerequisite to start contribute to *libicedb* code is to have installed a [git](https://en.wikipedia.org/wiki/Git) client on your system. In principle you can install whatever git client you prefer (some of them even have a GUI), but this instruction will cover the ordinary command-line client git which will be installed on ubuntu by typing on a terminal
```bash
sudo apt install git
```

## OPTIONAL (recommended): Setup SSH key pairs secure connection

GitHub allows you to upload your changes through a secure, encrypted SSH connection. SSH allows for many different authentication methods. Some institutions might require a two factor authentication which would make this step mandatory. In the case of GitHub, both server and client have public and private keys. To upload your changes to GitHub, you first have to generate your own SSH key, and then will upload the public key to GitHub. Follow github [guide](https://help.github.com/articles/connecting-to-github-with-ssh/) on how to generate and upload ssh key pairs.
{% include note.html content="You will need to generate and upload a ssh key pair for each system (desktops, laptops, servers and tablets) you are working on" %}

## Clone and branch the repository
If you want to contribute to the repository you first need to obtain the code and branch it.
We do not want to dive into the technicalities of git. To really understand what is going on you might want to look at the official

  1. The first time around execute
  ```bash
      git clone https://github.com/rhoneyager/libicedb.git
  ```
  
  This creates a new "libicedb" directory and you can go from there. For subsequent updates within the root "libicedb" directory, run
  ```bash
      git pull
  ```
  It will attempt to merge the latest changes with your copy and will warn you of any conflicts with your local version.

  2. You will have to do some first-time setup before committing to git. You only ever need to do this once.
  ```bash
     git config --global user.name "NAME SURNAME"
     git config --global user.email username@domain
     git config --global push.default simple
  ```

  3. With read-write access you'll want to place your changes in a separate "branch". For useful examples, see the related [tutorial page](https://services.github.com/on-demand/github-cli/clone-repo-cli)

     Enter the repository directory. We are going to make a new "branch" of the repository, named generically NEW-BRANCH-NAME, where you can commit your changes.
     ```bash
        cd PATH_TO_REPOSITORY
        git branch NEW-BRANCH-NAME
        git checkout NEW-BRANCH-NAME
        git branch --set-upstream-to origin
     ```
     For the seek of clearness you have first "created" a new branch out of the current branch (which would be the *master* branch if you followed these instructions) with the *git branch* command. Then you have "switched" to the new branch with *git checkout*. This does not produce visible effects because the new and the master branch are still identical. Finally *git branch --set-upstream* connects your "local" branch with the github server.
     
## Make your own changes.
  You might want to change some of the existing .hpp or .cpp files or to create new ones. Unfortunately it is not possible to make here a comprehensive reference on how to code.

  1. Tell git that you have changed files
     ```bash
        git add PATH_TO_MODIFIED_OR_NEW_FILE.cpp
     ```
     This must be repeated for every new or changed file. 
     
     **RELAX** You do not have to keep in mind all the files you have changed. If you want to have a clue of the *status* of your repository including the current untracked modifications just type
     ```bash
        git status
     ```
     
  2. Commit your changes to the repository. Write a good descriptive message describing what you did.
     ```bash
        git commit -m "A GOOD DESCRIPTIVE MESSAGE DESCRIBING WHAT I DID"
     ```
     Repeat this modify-add-commit cycle as many times as you wish as long as you keep in mind to make useful and descriptive commits. Keep in mind that all of your work till now is still on your local machine and nobody (including you from other systems) can see it.

  3. Replicate your changes back to GitHub. The psu-import branch does not yet exist on GitHub, so we will create it.
     ```bash
        git push origin NEW-BRANCH-NAME
     ```
     Now your work is syncronized with the upstream repository and everybody can see it. You might want to ask for review or comments of your current work from other developers that might be really helpful.
     
  4. Ask for merging your changes
  
     Once you are done with your work (you have implemented and tested a new feature) you might want to add this feature to the *master* branch of the code. **GO** to the GitHub website and create a "pull request" (https://github.com/rhoneyager/libicedb/pulls). This will send the other developers an email requesting for a review of the code and determine if it should be imported into the "master".
     
     [https://github.com/rhoneyager/libicedb/pulls](https://github.com/rhoneyager/libicedb/pulls) has a green button, labeled "New Pull Request". Click it.
        
     At the options at the top, make sure that you are applying branch "NEW-BRANCH-NAME" (compare) onto branch "master" (base).
     
     You might want to ask specific people (assign) to actually accept your changes or (review) to make comments, consider doing that from the menu on the right side of the pull request page.
        
     Click "Create pull request", type up any comments, and then click "Create pull request" again.
