---
layout: post
title: Markdown Test
date: Sep 6, 2018
summary: a demo for markdown styling
link: 2018-09-06-Markdown-Test
---

This post exists to demonstrate the Markdown styling used for posts. I've included the [Github stylesheet](https://github.com/sindresorhus/github-markdown-css) for very pretty post bodies. The only thing I've changed is the font, which is now Roboto. Some examples follow:

Here, we see a *block quote*:
> Mūsa, mihī causās memorā, quō nūmine laesō,
>
> quidve dolēns, rēgīna deum tot volvere cāsūs
>
> īnsīgnem pietāte virum, tot adīre labōrēs
>
> impulerit. Tantaene animīs caelestibus īrae?
>
> -- Vergil, <cite>The Aeneid</cite>

Here, we see a *code snippet*:
```
(define (fib n)
  (if (<= n 1)
    1
    (+ (fib (- n 1)) (fib (- n 2)))))
```

Here, we see a *table* ([data](https://w3techs.com/technologies/details/os-linux/all/all)):

| Distribution | % of Websites |
:------------- |:------------- |
| Ubuntu       | 36.2%         |
| Debian       | 23.4%         |
| CentOS       | 17.6%         |
| Gentoo       | 2.5%          |
| Red Hat      | 2.4%          |
| Fedora       | 0.6%          |

So, yeah. Not terribly interesting, just here to show off some of the technical capabilities of this Markdown-based blogging system.
