from typing import Any


class BaseMethod:
    def __init__(self, socket=None) -> None:
        self.socket = socket

    def __call__(self, payload=None) -> Any:
        return self.method(payload)

    def method(self, payload) -> Any:
        raise NotImplementedError("method function not implemented.")
