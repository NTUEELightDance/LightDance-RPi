from typing import Any


class BaseMethod():
    def __init__(self) -> None:
        pass
    def __call__(self, payload) -> Any:
        return self.method(payload)
    def method(self, payload) -> Any:
        raise NotImplementedError("method function not implemented.")