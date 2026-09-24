class LedgerSystem:
    def __init__(self):
        self.accounts = {}

    def CreateAccount(self, timestamp: int, accountId: str) -> str:
        if accountId in self.accounts:
            return "false"

        self.accounts[accountId] = 0
        return "true"

    def Deposit (self, timestamp: int, accountId: str, amount : int) -> str:
        if accountId not in self.accounts:
            return ""

        self.accounts[accountId] += amount
        return str(self.accounts[accountId])

    def Transfer (self, timestamp: int, sourceAccountId: str, targetAccountId: str,  amount : int) -> str:
        if sourceAccountId not in self.accounts or targetAccountId not in self.accounts:
            return "false"

        if targetAccountId == sourceAccountId:
            return "false"

        if self.accounts[sourceAccountId] < amount:
            return "false"

        self.accounts[sourceAccountId] -= amount
        self.accounts[targetAccountId] += amount
        return "true"
