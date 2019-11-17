.. Crafting a Interpreter in C++ documentation master file, created by
   sphinx-quickstart on Thu Nov 14 10:29:46 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

   notes from dan 

   I had to copy the breadcrumbs template into the _template folder and modify \
   the template to get the Docs text to become the home icon (in github repo this \
   has been done in a hopefullly soonto be released 0.4.4 sphinx_rtd_theme) 

Homepage
========

Acknowledgements
----------------

This book would not have been possible without Bob Nystrom's Crafting Interepreters. This book \
can be seen a seen as a companion piece to it to show how a similar implementation can be made \
using c++.  


.. hlist::

   
    * .. code-block:: cpp
     
        
         #include <vector>

         struct example {
            int a;
            float b;
            void example() {
                return;
            }
         }

         //this is a comment

         
         int main(){
         
             std::vector<int> myvec{1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7,8,9,1,2,3,4,
                                    5,6,7,8,9};
         }

    * this is an example of some code that we will want to talk about s

..  https://github.com/mgrachev/jekyll-syntax-railscasts

.. if :hidden: this tree will not be rendered in the main file on the homepage but will be \
   included in the toc on the side thanks to the read the docs theme 

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   codetest
