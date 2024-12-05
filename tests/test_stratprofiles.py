import pytest

import pygambit as gbt

from . import games


def test_remove_strategy():
    game = games.read_from_file("mixed_strategy.nfg")
    support_profile = game.support_profile()
    strategy = support_profile[0]
    new_profile = support_profile.remove(strategy)
    assert len(support_profile) == len(new_profile) + 1
    assert strategy not in new_profile


def test_difference():
    game = gbt.Game.new_table([2, 2])
    dif_profile = (
        game.support_profile()
        .remove(game.players[0].strategies[1])
        .remove(game.players[1].strategies[1])
    )
    new_profile = game.support_profile() - dif_profile
    assert len(new_profile) == 2
    for player in game.players:
        assert player.strategies[0] not in new_profile


def test_difference_error():
    game = gbt.Game.new_table([3, 2])
    support_profile = game.support_profile()
    dif_profile = game.support_profile().remove(game.players[1].strategies[0])
    with pytest.raises(ValueError):
        _ = dif_profile - support_profile


def test_intersection():
    game = gbt.Game.new_table([3, 2])
    fir_profile = game.support_profile().remove(game.players[1].strategies[0])
    sec_profile = game.support_profile().remove(game.players[0].strategies[2])
    new_profile = fir_profile & sec_profile
    assert len(new_profile) == 3
    assert new_profile <= sec_profile
    assert new_profile <= fir_profile


def test_intersection_error():
    game = gbt.Game.new_table([3, 2])
    fir_profile = game.support_profile().remove(game.players[1].strategies[0])
    sec_profile = game.support_profile().remove(game.players[1].strategies[1])
    with pytest.raises(ValueError):
        _ = fir_profile & sec_profile


def test_union():
    game = games.read_from_file("mixed_strategy.nfg")
    support_profile = game.support_profile()
    sec_profile = support_profile.remove(support_profile[4])
    new_profile = support_profile | sec_profile
    assert new_profile == support_profile


def test_undominated():
    game = games.read_from_file("mixed_strategy.nfg")
    support_profile = game.support_profile()
    new_profile = support_profile
    loop_profile = gbt.supports.undominated_strategies_solve(new_profile)
    while loop_profile != new_profile:
        new_profile = loop_profile
        loop_profile = gbt.supports.undominated_strategies_solve(new_profile)
    assert len(loop_profile) == 2
    assert list(loop_profile) == [support_profile[0], support_profile[3]]


def test_remove_error():
    game = games.read_from_file("mixed_strategy.nfg")
    support_profile = game.support_profile()
    profile = support_profile.remove(support_profile[3])
    with pytest.raises(gbt.UndefinedOperationError):
        profile.remove(profile[3])
