import subprocess
from typing import List

# Constants for the test configuration
BINARY_PATH: str = "./codexion"
ERROR_MSG: str = "ERROR : Wrong value detected during parsing. Aborting" \
     " programm !\n"
MISSING_ARGS: str = "ERROR : The number of arguments provided is incorrect."


def run(args: List[str]) -> subprocess.CompletedProcess:
    """
    Executes the codexion binary with the provided list of arguments.
    Captures both the standard output and the return code.
    """
    return subprocess.run(
        [BINARY_PATH] + args,
        capture_output=True,
        text=True
    )


def test_valid_arguments_fifo() -> None:
    """Tests the parsing logic with valid arguments and the FIFO scheduler."""
    result = run(["5", "800", "200", "200", "200", "7", "10", "fifo"])
    assert result.returncode == 0
    assert result.stderr == ""


def test_valid_arguments_edf() -> None:
    """Tests the parsing logic with valid arguments and the EDF scheduler."""
    result = run(["3", "1000", "300", "300", "300", "5", "15", "edf"])
    assert result.returncode == 0
    assert result.stderr == ""


def test_missing_arguments() -> None:
    """
    Tests parsing by providing fewer arguments than required (7 instead of 8).
    """
    result = run(["5", "800", "200", "200", "200", "7", "10"])
    assert result.returncode == 1
    assert result.stderr.startswith(MISSING_ARGS)


def test_too_many_arguments() -> None:
    """
    Tests parsing by providing more arguments than required (9 instead of 8).
    """
    result = run(
        ["5", "800", "200", "200", "200", "7", "10", "fifo", "extra_arg"]
        )
    assert result.returncode == 1
    assert result.stderr.startswith(MISSING_ARGS)


def test_negative_number() -> None:
    """
    Tests parsing by injecting a neg number, which should trigger an error.
    """
    result = run(["-5", "800", "200", "200", "200", "7", "10", "fifo"])
    assert result.returncode == 1
    assert result.stderr == ERROR_MSG


def test_non_digit_characters() -> None:
    """Tests parsing by mixing letters and numbers in a numeric field."""
    result = run(["5", "800a", "200", "200", "200", "7", "10", "fifo"])
    assert result.returncode == 1
    assert result.stderr == ERROR_MSG


def test_zero_coders() -> None:
    """Tests parsing with 0 coders, which is an invalid simulation state."""
    result = run(["0", "800", "200", "200", "200", "7", "10", "fifo"])
    assert result.returncode == 1
    assert result.stderr == ERROR_MSG


def test_invalid_scheduler() -> None:
    """Tests parsing with an unknown scheduler policy string."""
    result = run(["5", "800", "200", "200", "200", "7", "10", "roundrobin"])
    assert result.returncode == 1
    assert result.stderr == ERROR_MSG
