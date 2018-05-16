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

This is a step by step guide for whoever wants to start contributing to this documentation website and might have little experience with git and jekyll.
Unfortunately it is not meant to be general. It addresses immediately Ubuntu Linux users, but it is easy to extend to other OS.

## Clone and branch the repository

We assume you have installed a git client and have access to the github repository.
Following the next steps you will clone the repository to your machine, switch to the documentation branch `gh-pages` and make a new derivative branch of it.

```bash
git clone https://github.com/rhoneyager/libicedb.git
git checkout -b gh-pages-MYDOC gh-pages
git push -u origin gh-pages-MYDOC
```
This will create a new branch `gh-pages-MYDOC` derived from `gh-pages` which is the current state of the documentation. Also working directory is switched to the new branch and the upstream github repository is updated.

## Install and run Jekyll

This documentation website is hosted on [github](https://pages.github.com/) and the web engine behind it is called [jekyll](https://jekyllrb.com/)
The most effective way to contribute to the documentation is to install jekyll on your machine and run it locally so that you can see the rendering of the changes you make to the webpages.  
By running jekyll on your local machine you will have a direct evaluation of what your modification would look like on the website
Jekyll automatically recognize whenever you modify something in the file tree and updates the served html pages.

Install all of the requirements listed at the page [requirements](https://jekyllrb.com/docs/installation/#requirements) and follow the instructions to install the packages on your system. For Ubuntu linux user those are reported here for your convenience
```bash
sudo apt install ruby ruby-dev build-essential
echo '# Install Ruby Gems to ~/gems' >> ~/.bashrc
echo 'export GEM_HOME=$HOME/gems' >> ~/.bashrc
echo 'export PATH=$HOME/gems/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
gem install jekyll bundler
```

Be patient, it might take some time to install jekyll and bundler. We need some additional packages in order to run everything without troubles.  cd to the libicedb folder and run

```bash
bundle install
bundle update
```
This will download the github-pages layout that will make your local website looking exactly how it would appear once set upstream. Follow the instructions to install possible missing "gems" (gems are packages for the ruby programming language).

Now you should be ready to run jekyll on your system.  cd to the libicedb folder and run
```
bundle exec jekyll serve --baseurl /
```

Visit [http://127.0.0.1:4000/index.html](http://127.0.0.1:4000/index.html) on your preferred browser. You should see a copy of this website.

From now on, whenever you make a modification to the website files the jekyll server automatically updates the html and you should be able to se the modifications appearing on your browser after page refresh.

## Write a new page

### Add an entry on the side bar

This part may be skipped if you are willing to just add or modify on exhisting pages

Edit the file `_data/sidebars/mydoc_sidebar.yml` \\
The file as a tree structure (indentation matters). Add the new page informations here in the correct tree location
```yaml
  - title: Database File Structure
    output: web, pdf

    folderitems:
    - title: Metadata
      url: /structs_metadata.html
      output: web, pdf

    - title: Dimensions
      url: /structs_dimensions.html
      output: web, pdf
```

### Write the page

Write a new .md file under the folder /pages/mydoc/

Put in the header something like
```markdown
---
title: How to write documentation
tags: [getting_started, troubleshooting, collaboration]
last_updated: May 16, 2018
keywords: git, jekyll, documentation
summary: "Step by step guide for documentation contributors"
sidebar: mydoc_sidebar
permalink: writedoc.html
folder: mydoc
---
```
{% include note.html content="The permalink address has to match what you put in the sidebar url entry" %}

Write using [markdown formatting](https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet)

## Commit your changes and make a pull request
If you have added a new page
```bash
git add /PATH/TO/NEWPAGE.md
```
Repeat for each new page you have added since your last commit. Then commit your modifications and push them upstream.
```bash
git commit -am "A GOOD MESSAGE EXPLAINING YOUR MODIFICATIONS"
git push origin gh-pages-MYDOC
```
You will be asked for your github credentials

Visit [https://github.com/rhoneyager/libicedb/tree/gh-pages-MYDOC](https://github.com/rhoneyager/libicedb/tree/gh-pages-MYDOC) and clic on the button **New pull request**

On the top rectangle seclect `gh-pages` as the base branch. \\
Add a title and a description of your pull request.

OPTIONAL: use the menu on the right to ask for specific reviewers (from the list of contributors), label the request or assign it to a specific project.

## Troubleshooting
...
