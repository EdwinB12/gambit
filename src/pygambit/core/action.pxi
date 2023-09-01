#
# This file is part of Gambit
# Copyright (c) 1994-2022, The Gambit Project (http://www.gambit-project.org)
#
# FILE: src/python/gambit/lib/action.pxi
# Cython wrapper for actions
#
# This program is free software; you can redistribute it and/or modify
# It under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#

@cython.cclass
class Action:
    """A choice available at an information set."""
    action = cython.declare(c_GameAction)

    def __repr__(self):
        return (
            f"<Action [{self.action.deref().GetNumber()-1}] '{self.label}' "
            f"at infoset '{self.infoset.label}' "
            f"for player '{self.infoset.player.label}' "
            f"in game '{self.infoset.game.title}'>"
        )

    def __eq__(self, other: typing.Any) -> bool:
        return isinstance(other, Action) and self.action.deref() == cython.cast(Action, other).action.deref()

    def __ne__(self, other: typing.Any) -> bool:
        return not isinstance(other, Action) or self.action.deref() != cython.cast(Action, other).action.deref()

    def __hash__(self) -> int:
        return cython.cast(cython.long, self.action.deref())

    def delete(self):
        """Deletes this action from its information set.  The subtrees which
        are rooted at nodes that follow the deleted action are also deleted.

        Raises
        ------
        UndefinedOperationError
            If the action is the only action at its information set
        """
        if len(self.infoset.actions) == 1:
            raise UndefinedOperationError(
                "It is not possible to delete the last action of an infoset"
            )
        self.action.deref().DeleteAction()

    def precedes(self, node: Node) -> bool:
        """ Returns `True` if `node` precedes this action in the
        extensive game.

        Raises
        ------
        MismatchError
            If `node` is not in the same game as the action.
        """
        if self.infoset.game != node.game:
            raise MismatchError("precedes() requires a node from the same game as the action")
        return self.action.deref().Precedes(cython.cast(Node, node).node)

    @property
    def label(self) -> str:
        """Get or set the text label of the action."""
        return self.action.deref().GetLabel().decode('ascii')

    @label.setter
    def label(self, value: str) -> None:
        self.action.deref().SetLabel(value.encode('ascii'))

    @property
    def infoset(self) -> Infoset:
        """Get the information set to which the action belongs."""
        i = Infoset()
        i.infoset = self.action.deref().GetInfoset()
        return i

    @property
    def prob(self) -> typing.Union[decimal.Decimal, Rational]:
        """
        Get or set the probability a chance action is played.
        When setting the probability, the value can be any numeric type, or any object that
        has a string representation which can be interpreted as an integer,
        decimal, or rational number.

        Raises
        ------
        ValueError
            If `value` cannot be interpreted as a decimal or rational number.
        """
        py_string = cython.cast(
            string,
            self.action.deref().GetInfoset().deref().GetActionProb(self.action.deref().GetNumber())
        )
        if "." in py_string.decode('ascii'):
            return decimal.Decimal(py_string.decode('ascii'))
        else:
            return Rational(py_string.decode('ascii'))

    @prob.setter
    def prob(self, value: typing.Any) -> None:
        self.action.deref().GetInfoset().deref().SetActionProb(
            self.action.deref().GetNumber(), _to_number(value)
        )
