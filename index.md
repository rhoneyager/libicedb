---
title: "Getting started with icedb"
keywords: homepage icedb libicedb
tags: [getting_started]
sidebar: mydoc_sidebar
permalink: index.html
summary: icedb is a toolkit for manipulating particle scattering databases. It is under heavy development, so this site doesn't have much content yet. The instructions here will help you quickly get started. The other topics listed in the sidebar will provide additional information and detail.
---

{% include note.html content="If you're using this software, then you are probably interested in storing and manipulating snow and aerosol particles. This toolkit provides the ability to manipulate both particle structural information and scattering information." %}

## Get icedb pre-packaged 

On certain operating systems, pre-packaged versions of icedb are available. This is the easiest way to get started, but it is also the least customizable. If you only want to run the example binaries, then this option is best. If you want to link your own code against icedb, then this option is okay. If you want to develop icedb or greatly extend its functionality, then skip to the next section.

{% include note.html content="Packages are on the TO-DO list for now." %}

## Build icedb


Follow these instructions to build icedb.

### 1. Download icedb

First download or clone icedb from the [Github repo](https://github.com/rhoneyager/libicedb). Development is ongoing, so cloning the repository probably makes the most sense. However, for anyone unexperienced with git, GitHub or version control, you might just want to download a ZIP file containing the latest copy of the source code. In this case, click the **Clone or download** button, and then click **Download ZIP**.

{% include note.html content="Currently, this repository is **PRIVATE**, so only members of the ibedb development team can access it. If you want access, [send me an email](mailto:ryan@honeyager.info)." %}

### 2. Install the prerequisites

If you've never installed or run icedb locally on your computer, follow these instructions to install the backend libraries that icedb needs to function:

[Sidebar navigation][mydoc_sidebar_navigation].

* [Install dependencies on CentOS / RHEL / Fedora][install_deps_on_fed]
* [Install dependencies on Debian / Ubuntu][install_deps_on_deb]
* [Install dependencies on FreeBSD][install_deps_on_bsd]
* [Install dependencies on Mac][install_deps_on_mac]
* [Install dependencies on Windows][install_deps_on_windows]

### 3. Configure, build and install icedb using CMake

CMake is a tool for generating build scripts. Every development environment is set up in a different way. Files may be located in different places. Packages may have different versions. Different compilers may be used. Different operating systems provide different features to end-user programs. CMake accounts for all of these differences, and produces sets of building instructions that will work for your particular platform.

In case you have never used CMake before, you might want to take a look at [it's website](http://cmake.org) and these tutorials for end-users using the [console](https://www.youtube.com/watch?v=CLvZTyji_Uw) and a [GUI](https://www.youtube.com/watch?v=lAiuLHy4dCk).

If you've never installed or run icedb locally on your computer, follow these instructions to install icedb:


* [Build icedb using Makefiles (most of you should go here)][mydoc_build_icedb_on_makefiles]
* [Build icedb using Visual Studio 2017][mydoc_build_icedb_on_vs17]
* [Build icedb using XCode][mydoc_build_icedb_on_xcode]

### 4. Learn where the installation has put the different components of the icedb installation

### 5. Try out the example applications

### 6. Try linking a small, sample application to the icedb libraries



## Top navigation

The top navigation works just like the sidebar. You can specify which topnav data file should load by adding a `topnav` property in your page, like this:

```yaml
topnav: topnav
```

Here the topnav refers to the \_data/topnav.yml file.

Because most topnav options will be the same, the \_config.yml file specifies the topnav file as a default:

```yaml
-
  scope:
    path: ""
    type: "pages"
  values:
    layout: "page"
    comments: true
    search: true
    sidebar: home_sidebar
    topnav: topnav
```

## Sidebar syntax

The sidebar data file uses a specific YAML syntax that you must follow. Follow the sample pattern shown in the theme, specically looking at mydoc_sidebar.yml as an example: Here's a code sample showing all levels:

```yaml
entries:
- title: sidebar
  product: Jekyll Doc Theme
  version: 6.0
  folders:
  - title: Overview
    output: web, pdf
    folderitems:

    - title: Get started
      url: /index.html
      output: web, pdf
      type: homepage

    - title: Introduction
      url: /mydoc_introduction.html
      output: web, pdf

  - title: Release Notes
    output: web, pdf
    folderitems:

    - title: 6.0 Release notes
      url: /mydoc_release_notes_60.html
      output: web, pdf

    - title: 5.0 Release notes
      url: /mydoc_release_notes_50.html
      output: web, pdf

  - title: Tag archives
    output: web
    folderitems:

    - title: Tag archives overview
      url: /mydoc_tag_archives_overview.html
      output: web

      subfolders:
      - title: Tag archive pages
        output: web
        subfolderitems:

        - title: Formatting pages
          url: /tag_formatting.html
          output: web

        - title: Navigation pages
          url: /tag_navigation.html
          output: web

        - title: Content types pages
          url: /tag_content_types.html
          output: web
```

Each `folder` or `subfolder` must contain a `title` and `output` property. Each `folderitem` or `subfolderitem` must contain a `title`, `url`, and `output` property.

The two outputs available are `web` and `pdf`. (Even if you aren't publishing PDF, you still need to specify `output: web`).

The YAML syntax depends on exact spacing, so make sure you follow the pattern shown in the sample sidebars. See my [YAML tutorial](mydoc_yaml_tutorial) for more details about how YAML works.

{% include note.html content="If you have just one character of spacing off, Jekyll won't build due to the YAML syntax error. You'll see an error message in your console that says \"Error ... did not find expected key while parsing a block mapping at line 22 column 5. Error: Run jekyll build --trace for more information.\" If you encounter this, it usually refers to incorrect indentation or spacing in the YAML file. See the example mydoc_sidebar.yml file to see where your formatting went wrong." %}

Each level must have at least one topic before the next level starts. You can't have a second level that contains multiple third levels without having at least one standalone topic in the second level. If you need a hierarchy that has a folder that contains other folders and no loose topics, use a blank `-` item like this:

```yamll
entries:
- title: sidebar
  product: Jekyll Doc Theme
  version: 6.0
  folders:
  - title: Overview
    output: web, pdf
    folderitems:

    -

  - title: Release Notes
    output: web, pdf
    folderitems:

    - title: 6.0 Release notes
      url: /mydoc_release_notes_60.html
      output: web, pdf

    - title: 5.0 Release notes
      url: /mydoc_release_notes_50.html
      output: web, pdf

  - title: Installation
    output: web, pdf
    folderitems:

    - title: About Ruby, Gems, Bundler, etc.
      url: /mydoc_about_ruby_gems_etc.html
      output: web, pdf

    - title: Install Jekyll on Mac
      url: /mydoc_install_jekyll_on_mac.html
      output: web, pdf

    - title: Install Jekyll on Windows
      url: /mydoc_install_jekyll_on_windows.html
      output: web, pdf
```

To accommodate the title page and table of contents in PDF outputs, each product sidebar must list these pages before any other:

```yaml
- title:
  output: pdf
  type: frontmatter
  folderitems:
  - title:
    url: /titlepage
    output: pdf
    type: frontmatter
  - title:
    url: /tocpage
    output: pdf
    type: frontmatter
```

Leave the output as `output: pdf` for these frontmatter pages so that they don't appear in the web output.

For more detail on the sidebar, see [Sidebar navigation][mydoc_sidebar_navigation] and [YAML tutorial][mydoc_yaml_tutorial].

## Relative links and offline viewing

This theme uses relative links throughout so that you can view the site offline and not worry about which server or directory you're hosting it. It's common with tech docs to push content to an internal server for review prior to pushing the content to an external server for publication. Because of the need for seamless transferrence from one host to another, the site has to use relative links.

To view pages locally on your machine (without the Jekyll preview server), they need to have the `.html` extension. The `permalink` property in the page's frontmatter (without surrounding slashes) is what pushes the files into the root directory when the site builds.

## Page frontmatter

When you write pages, include these same frontmatter properties with each page:

```yaml
---
title: "Some title"
tags: [sample1, sample2]
keywords: keyword1, keyword2, keyword3
last_updated: Month day, year
summary: "optional summary here"
sidebar: sidebarname
permalink: filename.html
---
```

(You will customize the values for each of these properties, of course.)

For titles, surrounding the title in quotes is optional, but if you have a colon in the title, you must surround the title with quotation marks. If you have a quotation mark inside the title, escape it first with a backlash `\`.

Values for `keywords` get populated into the metadata of the page for SEO.

Values for `tags` must be defined in your \_data/tags.yml list. You also need a corresponding tag file inside the tags folder that follows the same pattern as the other tag files shown in the tags folder. (Jekyll won't auto-create these tag files.)

If you don't want the mini-TOC to show on a page (such as for the homepage or landing pages), add `toc: false` in the frontmatter.

The `permalink` value should be the same as your filename and include the ".html" file extension.

For more detail, see [Pages][mydoc_pages].

## Configure the top navigation

The top navigation bar's menu items are set through the \_data/topnav.yml file. Use the top navigation bar to provide links for navigating from one product to another, or to navigate to external resources.

For external URLs, use `external_url` in the item property, as shown in the example topnav.yml file. For internal links, use `url` the same was you do in the sidebar data files.

Note that the topnav has two sections: `topnav` and `topnav_dropdowns`. The topnav section contains single links, while the `topnav_dropdowns` section contains dropdown menus. The two sections are independent of each other.

## Blogs / News

For blog posts, create your markdown files in the \_posts folder following the sample formats. Post file names always begin with the date (YYYY-MM-DD-title).

The news/news.html file displays the posts, and the news_archive.html file shows a yearly history of posts. In documentation, you might use the news to highlight product features outside of your documentation, or to provide release notes and other updates.

See [Posts][mydoc_posts] for more information.

{% include links.html %}
