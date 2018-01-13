Diggler code UI readme
===================
UI elements are supposed to be created using `UIManager::add`, which forwards parameters to the control's constructor in addition to the `UIManager` pointer.

Generally, elements' lifetime are managed by the manager. That means: **don't free the elements' pointers!** Never, ever. If you want to hide an element, use its `setVisible(false)` method, or delete it (permanently) using `UIManager::remove(element)`.
Hiding an element also disables any input to it, as the name does *not* imply.

Some elements are handled in a more "traditional" way: they are drawn manually by a function rather than automatically through `UIManager`, are created *via* `UIManager::create` and destroyed manually.

Making a control
----
* Your class **must** derive from `diggler::ui::Element` in order to be controlled by the `UIManager`.
* All your constructors must take a `UIManager*` as first argument, called `M` if sticking to the projet's code style. They must then call `Element(M)` in the initializer list.
* Override the `Element` methods you want, and implement the mandatory (pure virtual) `render()` one.
