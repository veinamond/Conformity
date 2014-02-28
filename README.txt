Readmy for Conformity_SAT@GitHub

This is an application that we use to generate SAT encodings for the
computational study of one discrete model of collective behaviour.

Main idea of the model can be described as follows:
- Suppose there is some set group of people, of whom we know their social
interconnections within this group
- There is some process or phenomena that requires each member of our group
to choose exactly one of alternatives (for example, to attend elections or
not, to vote for one party or for another etc.)
- What we see in real life (simplified to great extent but anyway) is that
in such situations people often tend to make their decision based on the
decisions of other people that are important for them (neighbourhood). Say, 
I decide to attend elections when I hear that all my family is going to do
this. And otherwise I decide not to attend it if no member of my family
considers it worthwile.
- We assume that people can switch their oppinions several times as a result
of communicating with their neighbourhood.
- In our model we consider discrete time and synchronous changes, so all
members of our group change their oppinions (if they do) simultaneously in 
these moments.

The model:

- Consider an oriented graph G(V,E) (V-vertexes, E-Edges). Vertexes represent
people in our group, edges represent their interconnections. 

- Consider discrete time t={0,...,k} and dynamics of oppinions in our group
in these moments.

- Each vertex v_i \in V has weight x_i(t) \in {0,1}.  If weight in some moment
t_q is x_i(t_q)=1 then we say that v_i decides to "act" on step t_q, otherwise we say that it decides "not to act".

- Also each vertex v_i \in V_i has special parameter r_i - conformity value.
r_i has the following meaning: if r_i verices in the neighbourhood of V_i
decide to "act" (corresponding x_j(t) = 1), then on the next step t+1 the
weight x_i(t+1) will be equal to 1. Otherwise it will be 0. Currently we
assume that there is no inertion, so x_i(t+1) doesn't depend on x_i(t).

- With each v_i we associate set V_i - this is our neighbourhood. 
Interesting fact is that we can construct it in various ways. 
Currently it is seen as V_i={v_j\in V:(v_j,v_i)\in E}. 

- Note that V_i and r_i do not depend on t.

This model makes it possible to study for example the following problem:
Given graph G(V,E), values of r_i and amount of time "steps" k we need to 
know if there is such distribution of oppinions at the start (t=0) that the
number of vertices that decide to "act" is less than some P, and in the
moment t=k the number of vertices that decide to "act" is more than some 
Q, Q>P. It means that given the social structure we want to know if 
distribution of oppinions can evolve to the desired one following its
natural course of actions from some starting point.

We can introduce two new types of vertices to this model:
- Agitators - vertices whose weight is always 1, so they always "act".
- Loyalists - vertices whose weight is always 0, so they always "don't act".

These detalisations make it possible to consider quite peculiar problems:
Given graph G(V,E), values of r_i and amount of time "steps" k we need to 
know if there exists some distribution of agitators that (assuming that all
non-agitator vertices decide "not to act" at the start) can force the system
to evolve to desired configuration (usually with more than P vertices
striving to "act", but more complex conditions can be used).

Interconnection between agitators and loyalists make it possible to study
other interesting properties of such systems.

TODO list (from 28.02.2014)

1. Make it possible to weigh edges. It naturally corresponds to weights of 
oppinions of people from our neighbourhood. Say, I tend to trust more to 
members of my family (parents, brothers, sisters) than to members of my
sisters husband family. 

2. Introduce neighbourhoods of greater radius (friend of friends, friends of friends of friends), possibly with lesser weights. Is tied with 1.


