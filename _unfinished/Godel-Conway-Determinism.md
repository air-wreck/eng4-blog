---
layout: post
title: Gödel, Conway, and Determinism
date: Sep 17, 2018
summary: a musing on computers and philosophy
link: Godel-Conway-Determinism
---

(Yes, the post title was supposed to sound [like this](https://archive.org/details/GEBen_201404). I haven't read it, but I've heard it's very good.)

(Also, if you're not into naïve teenage musings, you might as well leave now.)

The subject of this first (substantive) post is perhaps not the first thing that comes to mind when thinking about computers. Actually, I wanted to talk a bit about the whole fate vs. free will thing we had going on in junior year, because I think an interesting connection can be made to computer science. I suppose it's a good thing that I haven't implemented a comment system on this blog (cf. the [War of Devolution](https://en.wikipedia.org/wiki/War_of_Devolution) on Edublogs during freshman year).

---
## Premise
Without getting into the probabilistic nature of quantum and statistical mechanics (with which I will freely admit that I am not terribly acquainted), we can roughly say that the premise of science is that the behavior of the natural world proceeds from a set of fundamental laws; moreover, it is possible to learn about these laws through observation and experiment. This immediately poses a problem for the philosopher: if we ourselves are members of the natural world and are therefore subject to its laws, are our actions then predestined? In this post, I would like to connect this problem with two well-known things from formal logic and computer science: Gödel's incompleteness theorem and Conway's game of life. This won't result in a satisfying answer to this vexing philosophical problem, but it's hopefully interesting to see the relationship here.

---
## Gödel's Incompleteness Theorem
As my eighth grade geometry class was learning about theorems and proofs, we once asked our teacher, the great man "Pappa John" Vallo, what his favorite theorem was. He responded with Gödel's incompleteness theorem(s) (actually, there are two closely related ones). Roughly (i.e. to the best of my mathematical ability), the (first) theorem says that:

> An axiomatic system powerful enough to describe the arithmetic of integers cannot be both complete and consistent.

Essentially, this means that if we have some sufficiently powerful system (a set of "laws" and a method of proving the truth or falsity of given statements with respect to those "laws"), then this system must be either incomplete (there are true statements that cannot be proven true under this system) or inconsistent (two contradictory statements are both found to be true under this system). Moreover, this system cannot be proven to be consistent under itself.

When Kurt Gödel first published this result, it was quite the surprise. At the time, mathematicians (most notably David Hilbert) had been undertaking one of the most ambitious projects of all time: they were trying to discover a set of basic mathematical "laws" from which all the rest of mathematics could be derived. They were seeking the very foundation of mathematics. Gödel showed up and literally proved that it was impossible.

This is also somewhat upsetting for physicists. Perhaps the holy grail of physics is the discovery of a so-called "theory of everything" (incidentally, Hilbert was also involved in this; it was the sixth of his famous list of open problems) that can predict the entirety of the natural world from a basic set of laws. There are various arguments for and against the possibility of such a theory, but one of the strongest against it is Gödel's theorem: surely any axiomatic basis for physics would be powerful enough to express basic arithmetic; wouldn't this imply that there can be no one set of (consistent) physical principles that can predict the entirety of natural phenomena? Given any set of "fundamental" laws of nature, shouldn't there always be phenomena that cannot be predicted under those laws?

It is immediately evident how this ties into the premise of this post. Might formal logic provide a way out of this philosophical problem?

---
## Conway's Game of Life
Up until now, this post has had little to do with the supposed theme of this blog: computers. I'll rectify that now. A lot of the CS education here in the US has muddled it up (but that's a post for another time), but "computer science" is ultimately the study of computers--i.e. things that compute. When studying such a science, it is natural to ask oneself: *gee, what sorts of things are computable?* As it turns out, this is the subject of one of the great foundational principles of computer science: the Church-Turing Thesis. Informally, the thesis says that:

> These three definitions of "computability" are equivalent:
> 1. The class of general recursive functions
> 2. The universal Turing machine
> 3. The lambda calculus

While recursive functions and the lambda calculus are fascinating (maybe a subject for another blog post?), the definition everyone loves is the Turing machine: a device that manipulates an infinite tape of ones and zeros. Despite the simplicity of the machine, it is capable of carrying out any computation possible. This lends itself to the notion of "Turing-completeness": something is said to be Turing-complete if it can simulate a Turing machine (and is itself thus capable of expressing any possible computation). There are some things whose Turing-completeness we take for granted: ~~computers~~, ~~the C programming language~~... okay, that's (yet again) a subject for another post. But there are some things that would at first seem to not be Turing-complete but surprisingly are. An example of this is Conway's game of life.

Conway's game is the most famous example of cellular automata, a set of remarkably simple simulations based on a grid of squares ("cells"). In Conway's game, the grid starts with a few shaded ("living") cells. Then, with each "tick" in time, the board is transformed based on a few simple rules (sourced from [Wikipedia](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life#Rules)):

> 1. A live cell with <2 neighbors dies ("underpopulation")
> 2. A live cell with 2-3 neighbors survives
> 3. A live cell with >3 neighbors dies ("overpopulation")
> 4. A dead cell with =3 neighbors comes alive ("reproduction")

At first, this seems to be a trivial biological simulation. It's entirely deterministic and based on a simple set of four rules. How much complexity can such a system really have? Well, since it is really equivalent to a Turing machine, it is actually capable of expressing any possible computation. However, this also means that the known limitations on Turing machines also apply to it. For example, despite the simple, deterministic rules, it's impossible to look at any two states in Conway's game and determine if one leads to the other. The only thing we can do is play the game...if state A results in state B or if state A results in an infinite loop, we can be sure that about whether A -> B. But if this doesn't happen, it's mathematically impossible for us to prove if state B will occur somewhere along the road. It's also mathematically impossible to prove whether an infinite loop will occur, short of experiencing one while playing (this is related to the famous Halting Problem). To put this a little more formally:

> Let A and B be two states in Conway's game. Then, for any algorithm X that you come up with to determine if a given state A will result in B, I can always come up with a pair of states that will stump your algorithm.

So, how does this relate to the premise? Essentially, even in a fully deterministic world, it is not necessarily possible to have predictive power over future states. Even if there are deterministic physical laws governing the natural world, it might not matter: regardless, there is no "shortcut" between point A and point B; the only way to get there is to live through it. Of course, this is a very informal argument that plays fast-and-loose with logic, but it's interesting nonetheless. One might reasonably object, "But what if I were to simulate the world with double the timescale? Might I then be able to predict the future?" I would hypothesize that this would be impossible: for any such simulation to be complete, it would have to include itself, which leads to infinite recursion.

Of course, this doesn't get us out of the moral ramifications of predestination, the fate vs. free will debate. However, it just might get us out of the practical considerations.

---

### Footnotes
[Relevant XKCD](https://xkcd.com/505/)

Okay, the claims in this post aren't *entirely* true. For instance, some algorithms like Hashlife can do game of life faster than just stepping through. But I think my point still stands.
