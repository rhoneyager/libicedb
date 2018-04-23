---
title: How to write documentation
tags: [getting_started, troubleshooting, collaboration]
last_updated: January 27, 2018
keywords: git, jekyll, documentation
summary: "Step by step guide for documentation contributors"
sidebar: mydoc_sidebar
permalink: writedoc.html
folder: mydoc
---

This is a step by step guide for wants quikly to start contributing to this documentation in the most effective way.  
Unfortunately it not ment to be general. It addresses immediately Ubuntu Linux users, but it is easy to extend to other OS.  

This guide will give you a straight path to install jekyll and make a new page in the documentation.
By running jekyll on your local machine you will have a direct evaluation of what your modification would look like on the website
Jekyll automatically recognize whenever you modify something in the file tree and updates the served html pages


### 1) Clone and branch the repository

We assume you have installed a git client and have access to the github repository.  
Following the next steps you will clone the repository to your machine, switch to the documentation branch gh-pages and make a new derivative branch of it

...
,,,
...


### 2) Install and run Jekyll

This documentation website is hosted on github and the web engine behind it is called jekyll.  
The most effective way to contribute to the documentation is to install jekyll on your machine and run it locally so that you can see the rendering of the changes you make to the webpages.  

Install all of the requirements listed at the page [requirements](https://jekyllrb.com/docs/installation/#requirements)  
Be patient, it might take some time to install jekyll and bunlder

Now you should be ready to run jekyll on your system. cd to the libicedb folder and try

bundle exec jekyll serve

On Ubuntu 16.04 it does not work at first, but it tells you to run first

bundle install

and then run 

bundle update

to get the github-pages layout that will make your local website looking exactly how it would appear once set upstream

to install missing "gems" (gems are packages for the ruby programming language). After that try again the previous command

## Write a new page

### ad an entry on the side bar

This part may be skiped if you are willing to just add or modify on exhisting pages

Edit the file _data/sidebars/mydoc_sidebar.yml

The file as a tree structure (indentation matters). Add the new page here in the right location

### write a new page

Write a new file in the location ...

Put in the header ...

Write using markdown formatting, find hints about markdown here [...] 

## TROUBLESHOOTING

On my implementation running bundle install overwrite Gemfile.lock which is important for something, and messes with paths, so when I visit localhost I always get error 404

git checkout Gemfile.lock

to fix it
