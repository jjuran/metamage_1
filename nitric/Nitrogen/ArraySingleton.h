// ArraySingleton.h

#ifndef NITROGEN_ARRAYSINGLETON_H
#define NITROGEN_ARRAYSINGLETON_H

namespace Nitrogen
  {
   template < class Element >
   struct ArraySingleton0
     {
      static const int size = 0;
            
      static const Element *Array()
        {
         return 0;
        }
     };

   template < class Element, Element e0 >
   struct ArraySingleton1
     {
      static const int size = 1;
      
      static const Element item0 = e0;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1 >
   struct ArraySingleton2
     {
      static const int size = 2;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1, Element e2 >
   struct ArraySingleton3
     {
      static const int size = 3;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      static const Element item2 = e2;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1, item2
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1, Element e2, Element e3 >
   struct ArraySingleton4
     {
      static const int size = 4;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      static const Element item2 = e2;
      static const Element item3 = e3;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1, item2, item3
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4 >
   struct ArraySingleton5
     {
      static const int size = 5;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      static const Element item2 = e2;
      static const Element item3 = e3;
      static const Element item4 = e4;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1, item2, item3, item4
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5 >
   struct ArraySingleton6
     {
      static const int size = 6;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      static const Element item2 = e2;
      static const Element item3 = e3;
      static const Element item4 = e4;
      static const Element item5 = e5;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1, item2, item3, item4, item5
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5, Element e6 >
   struct ArraySingleton7
     {
      static const int size = 7;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      static const Element item2 = e2;
      static const Element item3 = e3;
      static const Element item4 = e4;
      static const Element item5 = e5;
      static const Element item6 = e6;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1, item2, item3, item4, item5, item6
           };
         return array;
        }
     };

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5, Element e6, Element e7 >
   struct ArraySingleton8
     {
      static const int size = 8;
      
      static const Element item0 = e0;
      static const Element item1 = e1;
      static const Element item2 = e2;
      static const Element item3 = e3;
      static const Element item4 = e4;
      static const Element item5 = e5;
      static const Element item6 = e6;
      static const Element item7 = e7;
      
      static const Element *Array()
        {
         static const Element array[size] =
           {
            item0, item1, item2, item3, item4, item5, item6, item7
           };
         return array;
        }
     };


   template < class Element,
              Element e0 = 0, Element e1 = 0, Element e2 = 0, Element e3 = 0,
              Element e4 = 0, Element e5 = 0, Element e6 = 0, Element e7 = 0 >
   struct ArraySingleton
      : ArraySingleton8< Element, e0, e1, e2, e3, e4, e5, e6, e7 >
      {};

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5, Element e6 >
   struct ArraySingleton< Element, e0, e1, e2, e3, e4, e5, e6, 0 >
      : ArraySingleton7< Element, e0, e1, e2, e3, e4, e5, e6 >
      {};

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5 >
   struct ArraySingleton< Element, e0, e1, e2, e3, e4, e5, 0, 0 >
      : ArraySingleton6< Element, e0, e1, e2, e3, e4, e5 >
      {};

   template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4 >
   struct ArraySingleton< Element, e0, e1, e2, e3, e4, 0, 0, 0 >
      : ArraySingleton5< Element, e0, e1, e2, e3, e4 >
      {};

   template < class Element, Element e0, Element e1, Element e2, Element e3 >
   struct ArraySingleton< Element, e0, e1, e2, e3, 0, 0, 0, 0 >
      : ArraySingleton4< Element, e0, e1, e2, e3 >
      {};

   template < class Element, Element e0, Element e1, Element e2 >
   struct ArraySingleton< Element, e0, e1, e2, 0, 0, 0, 0, 0 >
      : ArraySingleton3< Element, e0, e1, e2 >
      {};

   template < class Element, Element e0, Element e1 >
   struct ArraySingleton< Element, e0, e1, 0, 0, 0, 0, 0, 0 >
      : ArraySingleton2< Element, e0, e1 >
      {};

   template < class Element, Element e0 >
   struct ArraySingleton< Element, e0, 0, 0, 0, 0, 0, 0, 0 >
      : ArraySingleton1< Element, e0 >
      {};
   
   template < class Element >
   struct ArraySingleton< Element, 0, 0, 0, 0, 0, 0, 0, 0 >
      : ArraySingleton0< Element >
      {};
  }

#endif
