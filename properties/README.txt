One of the main purposes of ECK is to exploit and make accessible the flexibility to model data that has been created by the introduction of the entity system. The Entity API (contrib module) has done a great job at making the entity system useful, but ECK attempts to build upon that.
One of the most useful things that I could think of in terms of flexibility, was the ability to add or take of way packages of functionality from an entity type. For example, if we wanted to make a certain entity type have a simple publishing system like nodes do. This functionality should be simple to package and make accessible to the users to activate or deactivate with ease.
In this folder I am adding a couple of modules that will do exactly this. 

I am hoping to create a title property module that will behave like node titles.
The module will allos for the entity title to be set during creation and modify during edition, when that entity is going to be viewed, the title will be the pages title, and the title property will be set as the label propety so that we can have autocomplete functionality with ease with a module like entity_autocomplete.

The second property module will implement a simple publishing workflow like the one for entities, so an entity can be hidden until published. It will probably not be as sophisticated as the publishing system in node, but the whole purpose is to show the capabilities of packaging this functionality in ways that it can easily be reused, and shared.
